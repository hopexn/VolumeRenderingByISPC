typedef float<3> float3;
typedef float<2> float2;
typedef int<3> int3;

inline static
bool intersect(uniform float3 cam_pos, float3 ray_dir, uniform float3 bmin, uniform float3 bmax){
    float t;
    float3 pointOnPlane;

    if (abs(ray_dir.x) > 1e-6) {
        if (ray_dir.x > 0) {
            t = (bmin.x - cam_pos.x) / ray_dir.x;
        } else {
            t = (bmax.x - cam_pos.x) / ray_dir.x;
        }
        if (t >= 0) {
            pointOnPlane = cam_pos + ray_dir * t;
            if (pointOnPlane.y >= bmin.y && pointOnPlane.y <= bmax.y &&
                pointOnPlane.z >= bmin.z && pointOnPlane.z <= bmax.z) {
                return true;
            }
        }
    }

    if (abs(ray_dir.y) > 1e-6) {
        if (ray_dir.y > 0) {
            t = (bmin.y - cam_pos.y) / ray_dir.y;
        } else {
            t = (bmax.y - cam_pos.y) / ray_dir.y;
        }
        if (t >= 0) {
            pointOnPlane = cam_pos + ray_dir * t;
            if (pointOnPlane.x >= bmin.x && pointOnPlane.x <= bmax.x &&
                pointOnPlane.z >= bmin.z && pointOnPlane.z <= bmax.z) {
                return true;
            }
        }
    }

    if (abs(ray_dir.z) > 1e-6) {
        if (ray_dir.z > 0) {
            t = (bmin.z - cam_pos.z) / ray_dir.z;
        } else {
            t = (bmax.z - cam_pos.z) / ray_dir.z;
        }
        if (t >= 0) {
            pointOnPlane = cam_pos + ray_dir * t;
            if (pointOnPlane.x >= bmin.x && pointOnPlane.x <= bmax.x &&
                pointOnPlane.y >= bmin.y && pointOnPlane.y <= bmax.y) {
                return true;
            }
        }
    }

    return false;
}


float2 getRayDistance(float3 cam_pos, float3 ray_dir, float3 bmin, float3 bmax) {
    float3 tmin = {-1e20, -1e20, -1e20};
    float3 tmax = {+1e20, +1e20, +1e20};

    if (abs(ray_dir.x) > 1e-6) {
        if (ray_dir.x > 0) {
            tmin.x = (bmin.x - cam_pos.x) / ray_dir.x;
            tmax.x = (bmax.x - cam_pos.x) / ray_dir.x;
        } else {
            tmax.x = (bmin.x - cam_pos.x) / ray_dir.x;
            tmin.x = (bmax.x - cam_pos.x) / ray_dir.x;
        }
    }

    if (abs(ray_dir.y) > 1e-6) {
        if (ray_dir.y > 0) {
            tmin.y = (bmin.y - cam_pos.y) / ray_dir.y;
            tmax.y = (bmax.y - cam_pos.y) / ray_dir.y;
        } else {
            tmax.y = (bmin.y - cam_pos.y) / ray_dir.y;
            tmin.y = (bmax.y - cam_pos.y) / ray_dir.y;
        }
    }

    if ((ray_dir.z) > 1e-6) {
        if (ray_dir.z > 0) {
            tmin.z = (bmin.z - cam_pos.z) / ray_dir.z;
            tmax.z = (bmax.z - cam_pos.z) / ray_dir.z;
        } else {
            tmax.z = (bmin.z - cam_pos.z) / ray_dir.z;
            tmin.z = (bmax.z - cam_pos.z) / ray_dir.z;
        }
    }

    float2 res;
    res.x = max(tmin.x, max(tmin.y, tmin.z));
    res.y = min(tmax.x, min(tmax.y, tmax.z));

    if (res.x > res.y) {
        res.x = 0;
        res.y = 0;
    }

    return res;
}

inline static
float read(float3 pos, uniform float3 bmin, uniform float3 bsize, uniform int3 dims, uniform int data[]){
    float3 relative = pos - bmin;
    relative /= bsize;
    relative *= dims;

    int3 index = relative;
    if(index.x < 0 || index.x >= dims.x  || index.y < 0 || index.y >= dims.y || index.z < 0 || index.z >= dims.z - 1){
        return 0;
    }

    float3 fraction = relative - index;

    int base = ((index.z * dims.y + index.y) * dims.x + index.x);
    uniform int xNext = 1;
    uniform int yNext = dims.x;
    uniform int zNext = dims.x * dims.y;

    int f000, f001, f010, f011, f100, f101, f110, f111;
    f000 = data[base];
    f001 = data[base + xNext];
    f010 = data[base + yNext];
    f011 = data[base + yNext + xNext];
    f100 = data[base + zNext];
    f101 = data[base + zNext + xNext];
    f110 = data[base + zNext + yNext];
    f111 = data[base + zNext + yNext + xNext];

    return  f000 * (1 - fraction.z) * (1 - fraction.y) * (1 - fraction.x) +
            f001 * (1 - fraction.z) * (1 - fraction.y) * (fraction.x)+
            f010 * (1 - fraction.z) * (fraction.y) * (1 - fraction.x) +
            f011 * (1 - fraction.z) * (fraction.y) * (fraction.x) +
            f100 * (fraction.z) * (1 - fraction.y) * (1 - fraction.x) +
            f101 * (fraction.z) * (1 - fraction.y) * (fraction.x) +
            f110 * (fraction.z) * (fraction.y) * (1 - fraction.x) +
            f111 * (fraction.z) * (fraction.y) * (fraction.x);
}

export void renderImage(uniform float cam[4][3],
    uniform float cam_plane_dist, uniform int width, uniform int height,
    uniform int _dims[3], uniform float bbox[3][3],
    uniform int data[], uniform float pixels[]){

    uniform float3  cam_pos  = {cam[0][0],  cam[0][1],  cam[0][2]};
    uniform float3  cam_dir  = {cam[1][0],  cam[1][1],  cam[1][2]};
    uniform float3  cam_up   = {cam[2][0],  cam[2][1],  cam[2][2]};
    uniform float3  cam_right= {cam[3][0],  cam[3][1],  cam[3][2]};
    uniform float3  bmin     = {bbox[0][0], bbox[0][1], bbox[0][2]};
    uniform float3  bmax     = {bbox[1][0], bbox[1][1], bbox[1][2]};
    uniform float3  bsize    = {bbox[2][0], bbox[2][1], bbox[2][2]};
    uniform int3    dims     = {_dims[0],   _dims[1],   _dims[2]};

    uniform int dims_max = max(dims[0], max(dims[1], dims[2]));
    uniform float stride = 1.f / dims_max;

    for(uniform int col = 0; col < width; col++){
        uniform float u = (col + 0.5f) / (width - 1)  - 0.5f;
        foreach(row = 0 ... height){
            float v = (row + 0.5f) / (height - 1) - 0.5f;
            float3 ray_dir = cam_dir * cam_plane_dist + cam_up * v + cam_right * u;
            float norm = sqrt(ray_dir.x * ray_dir.x + ray_dir.y * ray_dir.y + ray_dir.z * ray_dir.z);
            ray_dir /= norm;

            float gray = 0, a = 0;
            if(intersect(cam_pos, ray_dir, bmin, bmax)){
                float2 dist = getRayDistance(cam_pos, ray_dir, bmin, bmax);
                float beg = (int)(dist.x / stride) * stride;
                float end = dist.y;
                int count = (end - beg) / stride;
                float3 ray_pos = cam_pos + ray_dir * beg;
                float3 step = ray_dir * stride;
                for(int i = 0; i < count && a < 1; i++){
                    float g = read(ray_pos, bmin, bsize, dims, data) * stride;
                    float opacity_increment = (1 - a) * g;
                    gray += g * opacity_increment;
                    gray = min(gray, 1.f);
                    a += opacity_increment;
                    ray_pos = ray_pos + step;
                }
            }

            float bg = 1;
            gray = (gray * a + (1 - a) * bg) * 255;
            int index = row * width + col;
            pixels[index] = gray;
        }
    }
}