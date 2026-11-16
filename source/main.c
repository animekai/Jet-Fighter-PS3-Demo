#include <tiny3d.h>

#define PI 3.14159265f
#define JET_SLICES 7
#define ISL_SLICES 6
#define MAX_TRAIL 48

u32 jet_off[JET_SLICES], shad_off, isl_off[ISL_SLICES], cloud_off[3];
float trail_lx[MAX_TRAIL], trail_ly[MAX_TRAIL], trail_rx[MAX_TRAIL], trail_ry[MAX_TRAIL];
int trail_idx = 0;

const char *jet_art[JET_SLICES] = {
    {".......RR.......","......RRRR......","......RRRR......",".....RRRRRR.....",
     ".....BBBBBB.....","....BBBBBBBB....","...BBBBBBBBBB...","..BBBBBBBBBBBB..",
     ".RBBBBBBBBBBBBR.","RRBBBBBBBBBBBBRR","..BBBBBBBBBBBB..","....BBBBBBBB....",
     ".....BBBBBB.....",".....OO..OO.....","................","................"},
    {".......RR.......","......RRRR......","......RRRR......",".....DDDDDD.....",
     ".....DDDDDD.....","....DDDDDDDD....","...DDDDDDDDDD...","..DDDDDDDDDDDD..",
     ".RDDDDDDDDDDDDR.","RRDDDDDDDDDDDDRR","..DDDDDDDDDDDD..","....DDDDDDDD....",
     ".....DDDDDD.....",".....YY..YY.....","................","................"},
    {"................",".......RR.......","......RRRR......",".....DDDDDD.....",
     ".....CCCCCC.....","....CCCCCCCC....","...DDDDDDDDDD...","..DDDDDDDDDDDD..",
     ".WDDDDDDDDDDDDW.","WDDDDDDDDDDDDDDW","..DDDDDDDDDDDD..","....DDDDDDDD....",
     ".....DDDDDD.....","................","................","................"},
    {"................","................","................",".....CCCCCC.....",
     "....CCCCCCCC....","....CCCCCCCC....",".....DDDDDD.....","....DDDDDDDD....",
     "...DDDDDDDDDD...","....DDDDDDDD....",".....DDDDDD.....","....DDDDDDDD....",
     "................","................","................","................"},
    {"................","................","................","................",
     ".....CCCCCC.....",".....CCCCCC.....","......DDDD......","......DDDD......",
     "......DDDD......","......DDDD......",".....DDDDDD.....",".....DDDDDD.....",
     "................","................","................","................"},
    {"................","................","................","................",
     "................","................","................",".......DD.......",
     ".......DD.......",".......DD.......","......DDDD......","......DDDD......",
     "................","................","................","................"},
    {"................","................","................","................",
     "................","................","................","................",
     "................",".......RR.......",".......RR.......",".......RR.......",
     "................","................","................","................"}
};

u32 get_col(char c) {
    if (c == 'R') return 0xFFE63946;
    if (c == 'B') return 0xFF3A404A;
    if (c == 'D') return 0xFF23272E;
    if (c == 'C') return 0xFF4CC9F0;
    if (c == 'W') return 0xFFF1FAEE;
    if (c == 'O') return 0xFFF77F00;
    if (c == 'Y') return 0xFFFCBF49;
    return 0x00000000;
}

void init_textures() {
    u32 *ptr = tiny3d_AllocTexture(24 * 1024 * 1024);
    for (int s = 0; s < JET_SLICES; s++) {
        for (int y = 0; y < 16; y++)
            for (int x = 0; x < 16; x++)
                ptr[y * 16 + x] = get_col(jet_art[s][y][x]);
        jet_off[s] = tiny3d_TextureOffset(ptr); ptr += 256;
    }
    for (int y = 0; y < 16; y++)
        for (int x = 0; x < 16; x++)
            ptr[y * 16 + x] = (jet_art[0][y][x] != '.') ? 0x66081426 : 0;
    shad_off = tiny3d_TextureOffset(ptr); ptr += 256;

    for (int s = 0; s < ISL_SLICES; s++) {
        for (int y = 0; y < 128; y++) {
            for (int x = 0; x < 128; x++) {
                float dx = (x - 64.0f) / 64.0f, dy = (y - 64.0f) / 64.0f;
                float n = sinf(x * 0.09f) * cosf(y * 0.09f) * 0.24f + sinf((x - y) * 0.06f) * 0.14f;
                float d = sqrtf(dx * dx + dy * dy) + n;
                u32 col = 0;
                if (s == 0 && d < 0.78f) col = (d > 0.72f) ? 0xAAFFFFFF : 0xFF48CAE4;
                else if (s == 1 && d < 0.71f) col = 0xFFE9C46A;
                else if (s == 2 && d < 0.58f) col = (d > 0.54f) ? 0xFF52B788 : 0xFF74C69D;
                else if (s == 3 && d < 0.44f) col = (d > 0.40f) ? 0xFF40916C : 0xFF52B788;
                else if (s == 4 && d < 0.30f) col = ((x+y)%13==0) ? 0xFF8D99AE : 0xFF40916C;
                else if (s == 5 && d < 0.36f && ((x * 7 + y * 13) % 9 == 0)) col = 0xFF1B4332;
                ptr[y * 128 + x] = col;
            }
        }
        isl_off[s] = tiny3d_TextureOffset(ptr); ptr += 16384;
    }
    for (int s = 0; s < 3; s++) {
        float rad = 0.75f - s * 0.18f;
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 32; x++) {
                float dx = (x - 16.0f) / 16.0f, dy = (y - 16.0f) / 16.0f;
                float d = sqrtf(dx * dx + dy * dy) + sinf(x * 0.4f) * cosf(y * 0.4f) * 0.15f;
                ptr[y * 32 + x] = (d < rad) ? (s == 0 ? 0xFFD8E2DC : 0xFFFFFFFF) : 0;
            }
        }
        cloud_off[s] = tiny3d_TextureOffset(ptr); ptr += 1024;
    }
}

void draw_quad_rot(float x, float y, float z, float w, float h, float ang, float sx, float sy) {
    MATRIX m = MatrixRotationZ(ang);
    m = MatrixMultiply(m, MatrixTranslation(x, y, 0.0f));
    tiny3d_SetMatrixModelView(&m);
    float hw = w * 0.5f * sx, hh = h * 0.5f * sy;
    tiny3d_SetPolygon(TINY3D_QUADS);
    tiny3d_VertexPos(-hw, -hh, z); tiny3d_VertexColor(0xFFFFFFFF); tiny3d_VertexTexture(0.0f, 0.0f);
    tiny3d_VertexPos( hw, -hh, z); tiny3d_VertexTexture(0.99f, 0.0f);
    tiny3d_VertexPos( hw,  hh, z); tiny3d_VertexTexture(0.99f, 0.99f);
    tiny3d_VertexPos(-hw,  hh, z); tiny3d_VertexTexture(0.0f, 0.99f);
    tiny3d_End();
    tiny3d_SetMatrixModelView(NULL);
}

void draw_line_2d(float x1, float y1, float x2, float y2, float z, u32 col) {
    tiny3d_SetPolygon(TINY3D_LINES);
    tiny3d_VertexPos(x1, y1, z); tiny3d_VertexColor(col);
    tiny3d_VertexPos(x2, y2, z);
    tiny3d_End();
}

s32 main(s32 argc, const char* argv[]) {
    padInfo padinfo; padData paddata;
    tiny3d_Init(1024 * 1024); ioPadInit(7); init_textures();
    float px = 0, py = 0, ang = -PI * 0.25f, alt = 38.0f, roll = 0, speed = 2.6f;
    float cur_steer = 0.0f, cur_pitch = 0.0f, cur_thrust = 1.0f;
    int cam_mode = 0, btn_tri_prev = 0, btn_sq_prev = 0, frame_cnt = 0;

    while (1) {
        ioPadGetInfo(&padinfo);
        float target_steer = 0, target_pitch = 0, target_thrust = 1.0f;
        for (int i = 0; i < MAX_PADS; i++) {
            if (padinfo.status[i]) {
                ioPadGetData(i, &paddata);
                if (paddata.BTN_START || paddata.BTN_CROSS) return 0;
                if (paddata.ANA_L_H > 0 || paddata.ANA_L_V > 0) {
                    float lx = ((float)paddata.ANA_L_H - 128.0f) / 128.0f;
                    float ly = ((float)paddata.ANA_L_V - 128.0f) / 128.0f;
                    if (fabsf(lx) > 0.22f) target_steer = lx;
                    if (fabsf(ly) > 0.22f) target_pitch = -ly;
                }
                if (paddata.BTN_LEFT) target_steer = -1.0f;
                if (paddata.BTN_RIGHT) target_steer = 1.0f;
                if (paddata.BTN_UP) target_pitch = 1.0f;
                if (paddata.BTN_DOWN) target_pitch = -1.0f;
                if (paddata.BTN_R2 || paddata.BTN_CIRCLE) target_thrust = 2.3f;
                if (paddata.BTN_SQUARE && !btn_sq_prev && roll <= 0.01f) roll = 2.0f * PI;
                if (paddata.BTN_TRIANGLE && !btn_tri_prev) cam_mode = (cam_mode + 1) % 3;
                btn_sq_prev = paddata.BTN_SQUARE; btn_tri_prev = paddata.BTN_TRIANGLE;
            }
        }
        cur_steer += (target_steer - cur_steer) * 0.14f;
        cur_pitch += (target_pitch - cur_pitch) * 0.14f;
        cur_thrust += (target_thrust - cur_thrust) * 0.10f;
        ang += cur_steer * 0.045f; alt += cur_pitch * 1.1f;
        if (alt < 12.0f) alt = 12.0f; if (alt > 95.0f) alt = 95.0f;
        if (roll > 0.0f) { roll -= 0.18f; if (roll < 0.0f) roll = 0.0f; }
        px += cosf(ang) * speed * cur_thrust; py += sinf(ang) * speed * cur_thrust;
        if (++frame_cnt % 2 == 0) {
            float wa = ang + PI * 0.5f, sp = 18.0f * cosf(roll) * (1.0f - fabsf(cur_steer) * 0.2f);
            trail_lx[trail_idx] = px + cosf(wa) * sp - cosf(ang) * 10.0f;
            trail_ly[trail_idx] = py + sinf(wa) * sp - sinf(ang) * 10.0f;
            trail_rx[trail_idx] = px - cosf(wa) * sp - cosf(ang) * 10.0f;
            trail_ry[trail_idx] = py - sinf(wa) * sp - sinf(ang) * 10.0f;
            trail_idx = (trail_idx + 1) % MAX_TRAIL;
        }
        tiny3d_Clear(0xFF2468B4, TINY3D_CLEAR_ALL);
        tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);
        tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
            TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,
            TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD);
        tiny3d_Project2D();

        float cam_ang = (cam_mode == 0) ? 0.0f : (-ang - PI * 0.5f);
        float tilt = (cam_mode == 0) ? 1.0f : (cam_mode == 1 ? 0.55f : 0.35f);
        float step = (cam_mode == 0) ? 2.2f : (cam_mode == 1 ? 6.0f : 9.0f);
        for (int ix = -1; ix <= 1; ix++) {
            for (int iy = -1; iy <= 1; iy++) {
                float wx = (floorf(px / 520.0f) + ix) * 520.0f - px, wy = (floorf(py / 520.0f) + iy) * 520.0f - py;
                float rx = wx * cosf(cam_ang) - wy * sinf(cam_ang), ry = (wx * sinf(cam_ang) + wy * cosf(cam_ang)) * tilt;
                for (int s = 0; s < ISL_SLICES; s++) {
                    tiny3d_SetTexture(0, isl_off[s], 128, 128, 512, TINY3D_TEX_FORMAT_A8R8G8B8, TEXTURE_NEAREST);
                    draw_quad_rot(424.0f + rx, 256.0f + ry - s * step, 100.0f - s, 340.0f, 340.0f, cam_ang, 1.0f, tilt);
                }
                for (int c = 0; c < 3; c++) {
                    tiny3d_SetTexture(0, cloud_off[c], 32, 32, 128, TINY3D_TEX_FORMAT_A8R8G8B8, TEXTURE_NEAREST);
                    draw_quad_rot(424.0f + rx + 90.0f, 256.0f + ry - 55.0f - c * step, 15.0f - c, 72.0f, 72.0f, cam_ang, 1.0f, tilt);
                }
            }
        }
        if (cam_mode != 2) {
            float jx = 424.0f, jy = (cam_mode == 0) ? 256.0f : 340.0f;
            float jang = (cam_mode == 0) ? (ang + PI * 0.5f) : 0.0f, bank = cosf(roll) * (1.0f - fabsf(cur_steer) * 0.25f);
            tiny3d_SetTexture(0, shad_off, 16, 16, 64, TINY3D_TEX_FORMAT_A8R8G8B8, TEXTURE_NEAREST);
            draw_quad_rot(jx + alt * 0.35f, jy + alt * 0.35f, 30.0f, 44.0f, 44.0f, jang, bank, 1.0f);
            for (int k = 1; k < MAX_TRAIL; k++) {
                int i1 = (trail_idx + k - 1) % MAX_TRAIL, i2 = (trail_idx + k) % MAX_TRAIL;
                float x1 = trail_lx[i1] - px, y1 = trail_ly[i1] - py, x2 = trail_lx[i2] - px, y2 = trail_ly[i2] - py;
                if (fabsf(x1 - x2) < 40.0f && fabsf(y1 - y2) < 40.0f) {
                    float rx1 = x1 * cosf(cam_ang) - y1 * sinf(cam_ang), ry1 = (x1 * sinf(cam_ang) + y1 * cosf(cam_ang)) * tilt - (alt * 0.4f);
                    float rx2 = x2 * cosf(cam_ang) - y2 * sinf(cam_ang), ry2 = (x2 * sinf(cam_ang) + y2 * cosf(cam_ang)) * tilt - (alt * 0.4f);
                    draw_line_2d(424.0f + rx1, jy + ry1, 424.0f + rx2, jy + ry2, 22.0f, 0xAAFFFFFF);
                }
            }
            for (int s = 0; s < JET_SLICES; s++) {
                tiny3d_SetTexture(0, jet_off[s], 16, 16, 64, TINY3D_TEX_FORMAT_A8R8G8B8, TEXTURE_NEAREST);
                draw_quad_rot(jx, jy - (alt * 0.4f) - s * (step * 0.85f), 20.0f - s, 48.0f, 48.0f, jang, bank, 1.0f);
            }
        }
        tiny3d_Flip();
    }
    return 0;
}
