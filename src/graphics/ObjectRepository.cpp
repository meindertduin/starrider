#include "ObjectRepository.h"

#include "../io/ObjReader.h"

namespace Graphics {

ObjectRepository::ObjectRepository() : m_texture_collection(128), m_mesh_collection(128) {}

ObjectRepository::~ObjectRepository() {
    for (auto object : m_game_objects) {
        delete[] object.transformed_vertices;
    }
}

RenderObject ObjectRepository::create_game_object(std::string obj_file, std::string texture_file) {
    auto texture_id = load_texture(texture_file);
    auto texture = m_texture_collection.get_value(texture_id);

    auto object_color = A565Color(0xFF, 0, 0, 0);

    auto mesh_id = load_mesh_from_obj(obj_file, {
        .poly_state = PolyStateActive,
        .poly_attributes = PolyAttributeTwoSided | PolyAttributeRGB24 |
            PolyAttributeShadeModeIntensityGourad | PolyAttributeShadeModeGouraud | PolyAttributeShadeModeTexture,
        .poly_color = object_color,
    });
    auto mesh = m_mesh_collection.get_value(mesh_id);

    auto objects_count = m_game_objects.size();
    RenderObject object { static_cast<int>(objects_count > 0 ? objects_count - 1 : 0) };

    object.textures.push_back(texture);
    object.mip_levels = std::log(texture->width) / std::log(2) + 1;

    for (int mip_level = 1; mip_level < object.mip_levels; mip_level++) {
        auto quarter_texture = object.textures[mip_level - 1]->quarter_size(1.01f);
        object.textures.push_back(quarter_texture);
        auto id = m_texture_collection.store_value(std::unique_ptr<Texture>(quarter_texture));
        quarter_texture->id = id;
    }

    // TODO abstract the object creation
    object.state = ObjectStateActive | ObjectStateVisible;
    object.attributes |= ObjectAttributeSingleFrame;
    object.curr_frame = 0;
    object.frames_count = 1;

    object.vertex_count = mesh->vertex_count;
    object.local_vertices = mesh->vertices;
    object.alpha = 1.0f;

    object.transformed_vertices = new Vertex4D[mesh->vertex_count];

    object.text_count = mesh->text_count;
    object.texture_coords = mesh->text_coords;

    object.head_local_vertices = &object.local_vertices[0];
    object.head_transformed_vertices = &object.transformed_vertices[0];

    object.color = object_color;
    object.polygons = std::move(mesh->polygons);

    m_game_objects.push_back(object);
    return object;
}

RenderObject ObjectRepository::create_render_object(std::string mde_file) {
    auto object_color = A565Color(0xFF, 0, 0, 0);

    load_mesh_from_mde(mde_file, {
        .poly_state = PolyStateActive,
        .poly_attributes = PolyAttributeTwoSided | PolyAttributeRGB24 |
            PolyAttributeShadeModeIntensityGourad | PolyAttributeShadeModeGouraud | PolyAttributeShadeModeTexture,
        .poly_color = object_color,
    });


    auto mesh = m_mde_files.find(mde_file)->second;

    auto texture_id = load_texture("assets/" + mesh->skins[0]);
    auto texture = m_texture_collection.get_value(texture_id);

    auto objects_count = m_game_objects.size();
    RenderObject object { static_cast<int>(objects_count > 0 ? objects_count - 1 : 0) };

    object.textures.push_back(texture);
    object.mip_levels = std::log(texture->width) / std::log(2) + 1;

    for (int mip_level = 1; mip_level < object.mip_levels; mip_level++) {
        auto quarter_texture = object.textures[mip_level - 1]->quarter_size(1.01f);
        object.textures.push_back(quarter_texture);
        auto id = m_texture_collection.store_value(std::unique_ptr<Texture>(quarter_texture));
        quarter_texture->id = id;
    }

    // TODO abstract the object creation
    object.state = ObjectStateActive | ObjectStateVisible;
    object.attributes |= ObjectAttributeSingleFrame;
    object.curr_frame = 0;
    object.frames_count = 1;

    object.vertex_count = mesh->vertex_count;
    object.local_vertices = mesh->vertices;
    object.alpha = 1.0f;

    object.transformed_vertices = new Vertex4D[mesh->vertex_count];

    object.text_count = mesh->text_count;
    object.texture_coords = mesh->text_coords;

    object.head_local_vertices = &object.local_vertices[0];
    object.head_transformed_vertices = &object.transformed_vertices[0];

    object.color = object_color;
    object.polygons = std::move(mesh->polygons);

    m_game_objects.push_back(object);

    return object;
}

std::string  ObjectRepository::load_mesh_from_mde(std::string path, MeshAttributes attributes) {
    // create a new mesh from mde file if it does not exist
    if (m_mde_files.find(path) == m_mde_files.end()) {
        MdeReader reader;

        MdeFile file;
        reader.read_file(path, file);

        auto mesh = new Mesh {};

        for(int i = 0; i < file.header.num_skins; i++)
            mesh->skins.push_back(std::string(file.skins.get()[i]));

        mesh->vertex_count = file.header.num_verts;
        mesh->text_count = file.header.num_textcoords;

        mesh->vertices = new Vertex4D[file.header.num_verts * file.header.num_frames];
        mesh->text_coords = new Point2D[file.header.num_textcoords];

        std::vector<Graphics::Polygon> polygons;

        auto verts_ptr = file.verts.get();

        for (int iframe = 0; iframe < file.header.num_frames; iframe++) {
            for (int ivert = 0; ivert < file.header.num_verts; ivert++) {
                auto current_vert = &mesh->vertices[ivert * iframe];
                auto mde_vert = verts_ptr[ivert * iframe];

                current_vert->v = V4D {mde_vert.v[0], mde_vert.v[1], mde_vert.v[2], 0} ;
                current_vert->attributes = Graphics::VertexAttributePoint;
            }
        }

        auto text_coords_ptr = reinterpret_cast<Point2D*>(file.text_coords.get());
        std::copy(text_coords_ptr, text_coords_ptr + file.header.num_textcoords, mesh->text_coords);

        for (int iframe = 0; iframe < file.header.num_frames; iframe++) {
            for (int ipoly = 0; ipoly < file.header.num_polys; ipoly++) {
                Polygon polygon;
                auto mde_poly = file.polys.get()[iframe * ipoly];

                polygon.vertices = mesh->vertices;
                polygon.text_coords = mesh->text_coords;

                for (int j = 0; j < 3; j++) {
                    polygon.vert[j] = mde_poly.v_index[j];
                    polygon.state = attributes.poly_state;

                    polygon.attributes = attributes.poly_attributes;

                    if (file.header.num_textcoords > 0) {
                        polygon.vertices[mde_poly.v_index[j] * file.header.num_frames].t = mesh->text_coords[mde_poly.t_index[j]];

                        polygon.text[j] = mde_poly.t_index[j];
                        polygon.vertices[mde_poly.v_index[j] * file.header.num_frames].attributes |= Graphics::VertexAttributeTexture;
                    }

                    // TODO extract the normal indices
                }

                if (polygon.attributes & Graphics::PolyAttributeShadeModeGouraud ||
                        polygon.attributes & Graphics::PolyAttributeShadeModeIntensityGourad) {

                    auto line1 = mesh->vertices[polygon.vert[0]].v
                        - mesh->vertices[polygon.vert[1]].v;

                    auto line2 = mesh->vertices[polygon.vert[0]].v
                        - mesh->vertices[polygon.vert[2]].v;

                    polygon.n_length = line1.cross(line2).length();
                    polygon.text_coords = mesh->text_coords;
                }

                polygon.color = attributes.poly_color;

                polygons.push_back(polygon);
            }
        }

        mesh->polygons = polygons;

        // TODO: add way for checking normals
        compute_vertex_normals(*mesh);

        m_mde_files.insert(std::pair<std::string, Mesh*>(path, mesh));
    }

    return path;
}

int ObjectRepository::load_texture(std::string path) {
    auto texture = new Texture();
    if (!texture->load_from_bmp(path)) {
        return -1;
    }

    auto id = m_texture_collection.store_value(std::unique_ptr<Texture>(texture));
    texture->id = id;
    return id;
}

int ObjectRepository::load_mesh_from_obj(std::string path, MeshAttributes attributes) {
    ObjReader obj_reader;

    if (!obj_reader.read_file(path)) {
        return -1;
    }

    auto mesh = new Mesh {};

    mesh->vertex_count = obj_reader.m_vertices.size();
    mesh->text_count = obj_reader.m_tex_coords.size();

    mesh->vertices = new Graphics::Vertex4D[mesh->vertex_count];
    mesh->text_coords = new Graphics::Point2D[obj_reader.m_tex_coords.size()];

    for (int i = 0; i < mesh->vertex_count; i++) {
        mesh->vertices[i].v = obj_reader.m_vertices[i];
        mesh->vertices[i].attributes = Graphics::VertexAttributePoint;
    }

    std::copy(obj_reader.m_tex_coords.begin(), obj_reader.m_tex_coords.end(), mesh->text_coords);

    std::vector<Graphics::Polygon> polygons;
    for (int i = 0; i < obj_reader.m_indices.size(); i += 3) {
        Graphics::Polygon polygon;
        polygon.vertices = mesh->vertices;
        polygon.text_coords = mesh->text_coords;

        polygon.state = attributes.poly_state;

        polygon.attributes = attributes.poly_attributes;

        for (int j = 0; j < 3; j++) {
            auto current_index = obj_reader.m_indices[i + j];
            polygon.vert[j] = current_index.vertex_index;

            if (obj_reader.has_tex_coords) {
                polygon.vertices[current_index.vertex_index].t = mesh->text_coords[current_index.tex_coord_index];

                polygon.text[j] = current_index.tex_coord_index;

                polygon.vertices[current_index.vertex_index].attributes |= Graphics::VertexAttributeTexture;
            }

            if (obj_reader.has_normal_indices) {
                polygon.vertices[current_index.vertex_index].n = obj_reader.m_normals[current_index.normal_index];
                polygon.vertices[current_index.vertex_index].attributes |= Graphics::VertexAttributeNormal;
            }
        }

        if (polygon.attributes & Graphics::PolyAttributeShadeModeGouraud ||
                polygon.attributes & Graphics::PolyAttributeShadeModeIntensityGourad) {

            auto line1 = mesh->vertices[polygon.vert[0]].v
                - mesh->vertices[polygon.vert[1]].v;

            auto line2 = mesh->vertices[polygon.vert[0]].v
                - mesh->vertices[polygon.vert[2]].v;

            polygon.n_length = line1.cross(line2).length();
            polygon.text_coords = mesh->text_coords;
        }

        polygon.color = attributes.poly_color;

        polygons.push_back(polygon);
    }

    mesh->polygons = polygons;

    if (!obj_reader.has_normal_indices) {
        compute_vertex_normals(*mesh);
    }

    auto id = m_mesh_collection.store_value(std::unique_ptr<MeshType>(mesh));
    mesh->id = id;

    return id;
}

int ObjectRepository::compute_vertex_normals(Graphics::Mesh &object) {
    int polys_touch_vertices[Graphics::ObjectMaxVertices];
    memset((void*)polys_touch_vertices, 0, sizeof(int) * Graphics::ObjectMaxVertices);

    for (int poly = 0; poly < object.polygons.size(); poly++) {
        if (object.polygons[poly].attributes & Graphics::PolyAttributeShadeModeGouraud) {
            int vi0 = object.polygons[poly].vert[0];
            int vi1 = object.polygons[poly].vert[1];
            int vi2 = object.polygons[poly].vert[2];

            auto line1 = object.vertices[vi0].v
                - object.vertices[vi1].v;

            auto line2 = object.vertices[vi0].v
                - object.vertices[vi2].v;

            auto n = line1.cross(line2);

            object.polygons[poly].n_length = n.length();

            polys_touch_vertices[vi0]++;
            polys_touch_vertices[vi1]++;
            polys_touch_vertices[vi2]++;

            object.vertices[vi0].n += n;
            object.vertices[vi1].n += n;
            object.vertices[vi2].n += n;
        }
    }

    for (int vertex = 0; vertex < object.vertex_count; vertex++) {
        if (polys_touch_vertices[vertex] >= 1) {
            object.vertices[vertex].n /= polys_touch_vertices[vertex];
            object.vertices[vertex].n.normalise();
        }
    }

    return 1;
}

}
