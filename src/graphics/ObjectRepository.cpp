#include "ObjectRepository.h"

#include "../io/ObjReader.h"
#include "../io/MapReader.h"

#include "../assets/Cache.h"

#include <error.h>

namespace Graphics {

ObjectRepository::ObjectRepository() {}

ObjectRepository::~ObjectRepository() {
    for (auto object : m_game_objects)
        delete[] object.transformed_vertices;

    for (auto &mip_textures : m_skins)
        for (auto texture : mip_textures.second)
            delete texture;

    for (auto &mip_textures : m_textures)
        for (auto texture : mip_textures)
            delete texture;
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

    auto objects_count = m_game_objects.size();
    RenderObject object;

    object.textures = load_mip_texture("assets/" + mesh->skins[0]);

    object.mip_levels = object.textures.size();

    // TODO abstract the object creation
    object.state = ObjectStateActive | ObjectStateVisible;
    object.frames_count = mesh->frames_count;

    if (mesh->frames_count > 1) {
        object.attributes |= ObjectAttributeMultiFrame;
    } else {
        object.attributes |= ObjectAttributeSingleFrame;
    }

    object.curr_frame = 0;

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

RenderObject ObjectRepository::create_terrain_object(std::string path) {
    auto object_color = A565Color(0xFF, 0, 0, 0);

    load_mesh_from_map(path, {
        .poly_state = PolyStateActive,
        .poly_attributes = PolyAttributeRGB24 |
            PolyAttributeShadeModeIntensityGourad | PolyAttributeShadeModeGouraud | PolyAttributeShadeModeTexture,
        .poly_color = object_color,
    });

    auto mesh = m_mde_files.find(path)->second;

    auto objects_count = m_game_objects.size();
    RenderObject object;

    object.textures = load_mip_texture("assets/grass.bmp");

    object.mip_levels = object.textures.size();

    // TODO abstract the object creation
    object.state = ObjectStateActive | ObjectStateVisible;
    object.frames_count = mesh->frames_count;

    if (mesh->frames_count > 1) {
        object.attributes |= ObjectAttributeMultiFrame;
    } else {
        object.attributes |= ObjectAttributeSingleFrame;
    }

    object.curr_frame = 0;

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

string ObjectRepository::load_mesh_from_map(string path, MeshAttributes attributes) {
    if (m_mde_files.find(path) == m_mde_files.end()) {
        MapReader map_reader;
        MapFile mapfile;

        map_reader.read_file(path, mapfile);

        auto mesh = new Mesh {};

        mesh->frames_count = 1;

        mesh->vertex_count = mapfile.width * mapfile.farth;
        mesh->vertices = new Vertex4D[mesh->vertex_count];

        vector<Polygon> polygons;
        polygons.reserve((mapfile.width -1) * (mapfile.farth -1) * 2);

        mesh->text_count = 4;
        mesh->text_coords = new Point2D[4];

        mesh->text_coords[0] = Point2D { 0, 0 };
        mesh->text_coords[1] = Point2D { 1, 0 };
        mesh->text_coords[2] = Point2D { 0, 1 };
        mesh->text_coords[3] = Point2D { 1, 1 };

        for (int y = 0; y < mapfile.farth; y ++) {
            for (int x = 0; x < mapfile.width; x++) {
                auto v = &mesh->vertices[x + mapfile.width * y];
                v->v = Point4D(x, mapfile.ter_codes[x + mapfile.width *y].height / 5.0f, y);
                v->attributes = Graphics::VertexAttributePoint;
            }
        }

        for (int y = 0; y < mapfile.farth - 1; y++) {
            for (int x = 0; x < mapfile.width - 1; x++) {
                for (int ipoly = 0; ipoly < 2; ipoly++) {
                    Polygon polygon;

                    polygon.vertices = mesh->vertices;
                    polygon.text_coords = mesh->text_coords;

                    if (ipoly == 0) {
                        polygon.vert[2] = x + mapfile.width * y;
                        polygon.vert[1] = x + mapfile.width * y + 1;
                        polygon.vert[0] = x + mapfile.width * (y + 1);

                        polygon.text[2] = 0;
                        polygon.text[1] = 1;
                        polygon.text[0] = 2;
                    } else {
                        polygon.vert[0] = x + mapfile.width * y + 1;
                        polygon.vert[1] = x + mapfile.width * (y + 1);
                        polygon.vert[2] = x + mapfile.width * (y + 1) + 1;

                        polygon.text[0] = 1;
                        polygon.text[1] = 2;
                        polygon.text[2] = 3;
                    }

                    polygon.vertices[polygon.vert[0]].t = mesh->text_coords[polygon.text[0]];
                    polygon.vertices[polygon.vert[1]].t = mesh->text_coords[polygon.text[1]];
                    polygon.vertices[polygon.vert[2]].t = mesh->text_coords[polygon.text[2]];

                    polygon.attributes = attributes.poly_attributes;
                    polygon.state = attributes.poly_state;

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
        }

        mesh->polygons = std::move(polygons);

        // TODO: add way for checking normals
        compute_vertex_normals(*mesh);

        m_mde_files.insert(std::pair<std::string, Mesh*>(path, mesh));
    }

    return path;
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

        mesh->frames_count = file.header.num_frames;

        mesh->vertex_count = file.header.num_verts;
        mesh->text_count = file.header.num_textcoords;

        mesh->vertices = new Vertex4D[file.header.num_verts * file.header.num_frames];
        mesh->text_coords = new Point2D[file.header.num_textcoords];

        std::vector<Graphics::Polygon> polygons;

        auto verts_ptr = file.verts.get();

        for (int iframe = 0; iframe < file.header.num_frames; iframe++) {
            for (int ivert = 0; ivert < file.header.num_verts; ivert++) {
                auto vert_index = ivert + file.header.num_verts * iframe;
                auto current_vert = &mesh->vertices[vert_index];
                auto mde_vert = verts_ptr[vert_index];

                current_vert->v = V4D {mde_vert.v[0], mde_vert.v[1], mde_vert.v[2], 0} ;
                current_vert->attributes = Graphics::VertexAttributePoint;
            }
        }

        auto text_coords_ptr = reinterpret_cast<Point2D*>(file.text_coords.get());
        std::copy(text_coords_ptr, text_coords_ptr + file.header.num_textcoords, mesh->text_coords);

        auto polys_ptr = file.polys.get();

        for (int iframe = 0; iframe < file.header.num_frames; iframe++) {
            for (int ipoly = 0; ipoly < file.header.num_polys; ipoly++) {
                Polygon polygon;
                auto mde_poly = polys_ptr[ipoly];

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

std::vector<Texture*> ObjectRepository::load_mip_texture(std::string path) {
    auto mip_textures_pair = m_skins.find(path);

    if (mip_textures_pair == m_skins.end()) {
        std::vector<Texture*> mip_textures;

        auto root_texture = new Texture();
        if (!root_texture->load_from_bmp(path)) {
            throw std::runtime_error("Could not load bmp from file");
        }

        mip_textures.push_back(root_texture);

        auto mip_levels = std::log(root_texture->width) / std::log(2) + 1;

        for (int mip_level = 1; mip_level < mip_levels; mip_level++) {
            auto quarter_texture = mip_textures[mip_level - 1]->quarter_size(1.01f);
            mip_textures.push_back(quarter_texture);
        }

        m_skins.insert(std::pair<std::string, std::vector<Texture*>>(path, mip_textures));

        return mip_textures;
    }

    return mip_textures_pair->second;
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
