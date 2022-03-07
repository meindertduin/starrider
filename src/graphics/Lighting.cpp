#include "Lighting.h"

namespace Graphics {
int num_lights = 0;
Light g_lights[MaxLights];

void reset_lights() {
    std::memset(g_lights, 0, MaxLights * sizeof(Light));
    num_lights = 0;
}

int init_light(int index,
        uint16_t attributes, uint16_t state,
        RGBA c_ambient, RGBA c_diffuse, RGBA c_specular,
        Point4D pos,
        V4D dir,
        float kc, float kl, float kq,
        float pf)
{
    if (index < 0 || index >= MaxLights) {
        return -1;
    }

    g_lights[index].state = state;
    g_lights[index].id = index;
    g_lights[index].attributes = attributes;
    g_lights[index].c_ambient = c_ambient;
    g_lights[index].c_diffuse = c_diffuse;
    g_lights[index].c_specular = c_specular;

    g_lights[index].kc = kc;
    g_lights[index].kl = kl;
    g_lights[index].kq = kq;

    g_lights[index].pos = pos;
    g_lights[index].dir = dir.normalized();

    g_lights[index].pf = pf;

    num_lights++;

    return index;
}

int create_base_amb_light(int index, RGBA col) {
    return init_light(index, LightAttributeAmbient, LightStateOn,
            col, RGBA { 0 }, RGBA { 0 },
            V4D(), V4D(),
            0, 0, 0,
            0);
}

int create_base_dir_light(int index, RGBA col, V4D dir) {
    return init_light(index, LightAttributeInfinite, LightStateOn,
            RGBA { 0 }, col, RGBA { 0 },
            V4D(), dir,
            0, 1, 0,
            0);
}


int create_base_point_light(int index, RGBA col, V4D pos, float kc, float kl, float kq) {
    return init_light(index, LightAttributePoint, LightStateOn,
            RGBA { 0 }, col, RGBA { 0 },
            pos, V4D(),
            kc, kl, kq,
            0);
}

void gourad_light_polygon(RenderListPoly &polygon, Light *lights, int max_lights) {
    uint32_t r_base, g_base, b_base,
             r0_sum, g0_sum, b0_sum,
             r1_sum, g1_sum, b1_sum,
             r2_sum, g2_sum, b2_sum,
             ri, gi, bi,
             shaded_color;

    float dp, dist, n1, atten, i;

    polygon.color.rgb888_from_16bit(r_base, g_base, b_base);

    if (polygon.attributes & PolyAttributeShadeModeGouraud) {
        r0_sum = g0_sum = b0_sum = 0;
        r1_sum = g1_sum = b1_sum = 0;
        r2_sum = g2_sum = b2_sum = 0;

        for (int curr_light = 0; curr_light < max_lights; curr_light++) {
            if (!lights[curr_light].state) {
                continue;
            }

            if (lights[curr_light].attributes & LightAttributeAmbient) {
                ri = ((lights[curr_light].c_ambient.r * r_base) >> 8);
                gi = ((lights[curr_light].c_ambient.g * g_base) >> 8);
                bi = ((lights[curr_light].c_ambient.b * b_base) >> 8);

                r0_sum += ri;
                g0_sum += gi;
                b0_sum += bi;

                r1_sum += ri;
                g1_sum += gi;
                b1_sum += bi;

                r2_sum += ri;
                g2_sum += gi;
                b2_sum += bi;
            }
            else if (lights[curr_light].attributes & LightAttributeInfinite) {
                auto dp = polygon.trans_verts[0].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r0_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                    g0_sum += ((lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128));
                    b0_sum += ((lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128));
                }

                dp = polygon.trans_verts[1].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r1_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                    g1_sum += ((lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128));
                    b1_sum += ((lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128));
                }

                dp = polygon.trans_verts[2].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r2_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                    g2_sum += ((lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128));
                    b2_sum += ((lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128));
                }
            } else if (lights[curr_light].attributes * LightAttributePoint) {
                auto l = V4D(polygon.trans_verts[0].v, lights[curr_light].trans_pos);
                dist = l.length();
                dp = polygon.normal.dot(l);

                if (dp > 0) {
                    atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
                    i = 128 * dp / (polygon.normal.length() * dist * atten);

                    r0_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                    g0_sum += ((lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128));
                    b0_sum += ((lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128));
                }

                l = V4D(polygon.trans_verts[1].v, lights[curr_light].trans_pos);
                dist = l.length();
                dp = polygon.normal.dot(l);

                if (dp > 0) {
                    atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
                    i = 128 * dp / (polygon.normal.length() * dist * atten);

                    r1_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                    g1_sum += ((lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128));
                    b1_sum += ((lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128));
                }

                l = V4D(polygon.trans_verts[2].v, lights[curr_light].trans_pos);
                dist = l.length();
                dp = polygon.normal.dot(l);

                if (dp > 0) {
                    atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
                    i = 128 * dp / (polygon.normal.length() * dist * atten);

                    r2_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                    g2_sum += ((lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128));
                    b2_sum += ((lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128));
                }
            }
        }

        if (r0_sum > 255) r0_sum = 255;
        if (g0_sum > 255) g0_sum = 255;
        if (b0_sum > 255) b0_sum = 255;
        if (r1_sum > 255) r1_sum = 255;
        if (g1_sum > 255) g1_sum = 255;
        if (b1_sum > 255) b1_sum = 255;
        if (r2_sum > 255) r2_sum = 255;
        if (g2_sum > 255) g2_sum = 255;
        if (b2_sum > 255) b2_sum = 255;

        polygon.lit_color[0] = A565Color(r0_sum, g0_sum, b0_sum, 0xFF);
        polygon.lit_color[1] = A565Color(r1_sum, g1_sum, b1_sum, 0xFF);
        polygon.lit_color[2] = A565Color(r2_sum, g2_sum, b2_sum, 0xFF);
    }
}

void gourad_intensity_light_polygon(RenderListPoly &polygon, Light *lights, int max_lights) {
    uint32_t r_base, g_base, b_base,
             r0_sum,
             r1_sum,
             r2_sum,
             ri,
             shaded_color;

    polygon.color.rgb888_from_16bit(r_base, g_base, b_base);

    float dp, dist, n1, atten, i;

    if (polygon.attributes & PolyAttributeShadeModeIntensityGourad) {
        r0_sum = 0;
        r1_sum = 0;
        r2_sum = 0;

        for (int curr_light = 0; curr_light < max_lights; curr_light++) {
            if (!lights[curr_light].state) {
                continue;
            }

            if (lights[curr_light].attributes & LightAttributeAmbient) {
                ri = ((lights[curr_light].c_ambient.r * r_base) >> 8);

                r0_sum += ri;
                r1_sum += ri;
                r2_sum += ri;
            }
            else if (lights[curr_light].attributes & LightAttributeInfinite) {
                auto dp = polygon.trans_verts[0].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r0_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                }

                dp = polygon.trans_verts[1].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r1_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                }

                dp = polygon.trans_verts[2].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r2_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                }
            } else if (lights[curr_light].attributes * LightAttributePoint) {

                auto l = V4D(lights[curr_light].trans_pos, polygon.trans_verts[0].v);
                dist = l.length();
                dp = polygon.trans_verts[0].n.dot(l);

                if (dp > 0.0f) {
                    atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
                    i = 128 * dp / (dist * atten);

                    r0_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                }

                l = V4D(lights[curr_light].trans_pos, polygon.trans_verts[1].v);

                dist = l.length();
                dp = polygon.trans_verts[1].n.dot(l);

                if (dp > 0.0f) {
                    atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
                    i = 128 * dp / (dist * atten);

                    r1_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                }

                l = V4D(lights[curr_light].trans_pos, polygon.trans_verts[2].v);
                dist = l.length();
                dp = polygon.trans_verts[2].n.dot(l);

                if (dp > 0.0f) {
                    atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
                    i = 128 * dp / (dist * atten);

                    r2_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                }
            }
        }

        if (r0_sum > 255) r0_sum = 255;
        if (r1_sum > 255) r1_sum = 255;
        if (r2_sum > 255) r2_sum = 255;

        polygon.trans_verts[0].i = r0_sum / 255.0f;
        polygon.trans_verts[1].i = r1_sum / 255.0f;
        polygon.trans_verts[2].i = r2_sum / 255.0f;
    }
}

void flat_light_polygon(RenderListPoly &polygon, Light *lights, int max_lights) {
    uint32_t r_base, g_base, b_base,
             r_sum,
             ri,
             shaded_color;

    polygon.color.rgb888_from_16bit(r_base, g_base, b_base);

    float dp, dist, n1, atten, i;

    if (polygon.attributes & PolyAttributeShadeModeIntensityGourad) {
        r_sum = 0;

        for (int curr_light = 0; curr_light < max_lights; curr_light++) {
            if (!lights[curr_light].state) {
                continue;
            }

            if (lights[curr_light].attributes & LightAttributeAmbient) {
                ri = ((lights[curr_light].c_ambient.r * r_base) >> 8);

                r_sum += ri;
            }
            else if (lights[curr_light].attributes & LightAttributeInfinite) {
                auto dp = polygon.trans_verts[0].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                }
            } else if (lights[curr_light].attributes * LightAttributePoint) {
                auto l = V4D(lights[curr_light].trans_pos, polygon.trans_verts[0].v);
                dist = l.length();
                dp = polygon.trans_verts[0].n.dot(l);

                if (dp > 0.0f) {
                    atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
                    i = 128 * dp / (dist * atten);

                    r_sum += ((lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128));
                }
            }
        }

        if (r_sum > 255) r_sum = 255;

        auto intensity = r_sum / 255.0f;

        polygon.trans_verts[0].i = intensity;
        polygon.trans_verts[1].i = intensity;
        polygon.trans_verts[2].i = intensity;
    }
}
}
