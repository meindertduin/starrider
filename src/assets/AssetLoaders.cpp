#include "AssetLoaders.h"

#include "../io/BmpReader.h"
#include "../io/MdeReader.h"

#include "../graphics/Texture.h"

#include "Cache.h"

namespace Assets {
    bool load_texture(Cache &cache, const std::string &filename, const AssetOptions &options) {
        BmpReader reader;

        Graphics::MipTexturesList mip_textures;

        auto root_texture = new Graphics::Texture();
        if (!root_texture->load_from_bmp(filename)) {
            throw std::runtime_error("Could not load bmp from file");
        }

        mip_textures.push_back(std::unique_ptr<Graphics::Texture>(root_texture));

        auto mip_levels = std::log(root_texture->width) / std::log(2) + 1;

        for (int mip_level = 1; mip_level < mip_levels; mip_level++) {
            auto quarter_texture = mip_textures[mip_level - 1]->quarter_size(1.01f);
            mip_textures.push_back(std::move(quarter_texture));
        }

        cache.set_textures(filename, std::move(mip_textures));

        return true;
    }

    int compute_vertex_normals(Graphics::Mesh &object) {
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

    bool load_mde_file(Cache &cache, const std::string &filename, const AssetOptions &options) {
        MdeReader reader;

        MdeFile file;
        reader.read_file(filename, file);

        auto mesh = new Graphics::Mesh {};

        for(int i = 0; i < file.header.num_skins; i++)
            mesh->skins.push_back(std::string(file.skins.get()[i]));

        mesh->frames_count = file.header.num_frames;

        mesh->vertex_count = file.header.num_verts;
        mesh->text_count = file.header.num_textcoords;

        mesh->vertices = new Graphics::Vertex4D[file.header.num_verts * file.header.num_frames];
        mesh->text_coords = new Graphics::Point2D[file.header.num_textcoords];

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

        auto text_coords_ptr = reinterpret_cast<Graphics::Point2D*>(file.text_coords.get());
        std::copy(text_coords_ptr, text_coords_ptr + file.header.num_textcoords, mesh->text_coords);

        auto polys_ptr = file.polys.get();

        for (int iframe = 0; iframe < file.header.num_frames; iframe++) {
            for (int ipoly = 0; ipoly < file.header.num_polys; ipoly++) {
                Graphics::Polygon polygon;
                auto mde_poly = polys_ptr[ipoly];

                polygon.vertices = mesh->vertices;
                polygon.text_coords = mesh->text_coords;

                for (int j = 0; j < 3; j++) {
                    polygon.vert[j] = mde_poly.v_index[j];
                    polygon.state = options.mesh_options.poly_state;

                    polygon.attributes = options.mesh_options.poly_attributes;

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

                polygon.color = options.mesh_options.poly_color;

                polygons.push_back(polygon);
            }
        }

        mesh->polygons = polygons;

        // TODO: add way for checking normals
        compute_vertex_normals(*mesh);

        cache.set_mesh(filename, std::unique_ptr<Graphics::Mesh>(mesh));
    }
}
