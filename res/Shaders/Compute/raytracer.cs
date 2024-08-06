#version 430
layout(local_size_x = 16, local_size_y = 16, local_size_z = 4) in; // Adjust the sizes as needed


struct Ray {
    vec3 origin;
    vec3 direction;
};

vec3 point_at(Ray r, float t) {
    return r.origin + t * r.direction;
}


struct Material {
    int type;       
    vec3 albedo;
    float fuzz;   
    float ir;       
};

struct HitRecord {
    vec3 p;           
    vec3 normal;      
    Material material; 
    float t;         
    bool front_face;  
};

struct Sphere {
    vec3 center;
    float radius;
    Material material;
};

const float infinity = 1e20;

uniform vec3 camera_origin;
uniform vec3 camera_lower_left_corner;
uniform vec3 camera_horizontal;
uniform vec3 camera_vertical;
uniform vec3 ray00;
uniform vec3 ray01;
uniform vec3 ray10;
uniform vec3 ray11;
// Scene setup
uniform int num_spheres;
uniform Sphere spheres[2];

uniform int image_width = 1000;
uniform int image_height = 1000;
uniform int max_depth = 50;

layout(rgba32f, binding = 0) uniform image2D output_image;


float rand() {
    return fract(sin(gl_GlobalInvocationID.x * 12.9898 + gl_GlobalInvocationID.y * 78.233) * 43758.5453123);
}

vec3 random_unit_vector() {
    float a = rand() * 2.0 * 3.1415926535897932384626433832795; // Random angle
    float z = rand() * 2.0 - 1.0; // Random z-coordinate in range [-1, 1]
    float r = sqrt(1.0 - z * z);  // Compute radius
    return vec3(r * cos(a), r * sin(a), z); 
}



void set_face_normal(Ray r, vec3 outward_normal, out HitRecord rec) {
    rec.front_face = dot(r.direction, outward_normal) < 0.0;
    rec.normal = rec.front_face ? outward_normal : -outward_normal;
}

bool hit_sphere(Sphere sphere, Ray r, float t_min, float t_max, out HitRecord rec) {
    vec3 oc = r.origin - sphere.center;
    float a = dot(r.direction, r.direction);
    float half_b = dot(oc, r.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = half_b * half_b - a * c;

    if (discriminant > 0.0) {
        float sqrtd = sqrt(discriminant);
        float root = (-half_b - sqrtd) / a;
        if (root < t_max && root > t_min) {
            rec.t = root;
            rec.p = point_at(r, rec.t);
            vec3 outward_normal = (rec.p - sphere.center) / sphere.radius;
            set_face_normal(r, outward_normal, rec);
            rec.material = sphere.material;
            return true;
        }

        root = (-half_b + sqrtd) / a;
        if (root < t_max && root > t_min) {
            rec.t = root;
            rec.p = point_at(r, rec.t);
            vec3 outward_normal = (rec.p - sphere.center) / sphere.radius;
            set_face_normal(r, outward_normal, rec);
            rec.material = sphere.material;
            return true;
        }
    }
    return false;
}

bool hit_scene(Ray r, float t_min, float t_max, out HitRecord rec) {
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    // Check for each sphere in the scene
    for (int i = 0; i < num_spheres; i++) {
        if (hit_sphere(spheres[i], r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}





bool scatter_lambertian(Ray r_in, vec3 hit_point, vec3 normal, vec3 albedo, out Ray scattered, out vec3 attenuation) {
    vec3 scatter_direction = normal + random_unit_vector();  

    if (length(scatter_direction) < 0.001) {
        scatter_direction = normal;
    }

    scattered = Ray(hit_point, normalize(scatter_direction));
    attenuation = albedo;
    return true;
}

bool scatter_metal(Ray r_in, vec3 hit_point, vec3 normal, vec3 albedo, float fuzz, out Ray scattered, out vec3 attenuation) {
    vec3 reflected = reflect(normalize(r_in.direction), normal);
    scattered = Ray(hit_point, reflected + fuzz * random_unit_vector());
    attenuation = albedo;
    return dot(scattered.direction, normal) > 0.0;
}

float reflectance(float cosine, float ref_idx) {
    float r0 = (1.0 - ref_idx) / (1.0 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow((1.0 - cosine), 5.0);
}

bool scatter_dielectric(Ray r_in, vec3 hit_point, vec3 normal, float ir, out Ray scattered, out vec3 attenuation) {
    attenuation = vec3(1.0);  
    float refraction_ratio = dot(r_in.direction, normal) > 0.0 ? ir : (1.0 / ir);
    
    vec3 unit_direction = normalize(r_in.direction);
    float cos_theta = min(dot(-unit_direction, normal), 1.0);
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    
    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    vec3 direction;
    
    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > rand()) {
        direction = reflect(unit_direction, normal);
    } else {
        direction = refract(unit_direction, normal, refraction_ratio);
    }

    scattered = Ray(hit_point, direction);
    return true;
}



bool scatter(Material mat, Ray r_in, vec3 hit_point, vec3 normal, out Ray scattered, out vec3 attenuation) {
    if (mat.type == 0) {  // Lambertian
        return scatter_lambertian(r_in, hit_point, normal, mat.albedo, scattered, attenuation);
    } else if (mat.type == 1) {  // Metal
        return scatter_metal(r_in, hit_point, normal, mat.albedo, mat.fuzz, scattered, attenuation);
    } else if (mat.type == 2) {  // Dielectric
        return scatter_dielectric(r_in, hit_point, normal, mat.ir, scattered, attenuation);
    }
    return false;
}


// vec3 ray_color(Ray r, int depth) {

    
//     if (depth <= 0) {
//         return vec3(0.0);  
//     }

//     HitRecord rec;
//     if (hit_scene(r, 0.001, infinity, rec)) {
//         Ray scattered;
//         vec3 attenuation;
//         if (scatter(rec.material, r, rec.p, rec.normal, scattered, attenuation)) {
//             return attenuation * ray_color(scattered, depth - 1);
//         }
//         return vec3(0.0);
//     }

//     // Sky color if no hit
//     vec3 unit_direction = normalize(r.direction);
//     float t = 0.5 * (unit_direction.y + 1.0);
//     return mix(vec3(1.0), vec3(0.5, 0.7, 1.0), t);
// }

vec3 ray_color(Ray r) {
    vec3 color = vec3(0.0);
    vec3 attenuation = vec3(1.0);
    int depth = max_depth;

    while (depth > 0) {
        HitRecord rec;
        if (hit_scene(r, 0.001, infinity, rec)) {
            Ray scattered;
            vec3 temp_attenuation;
            if (scatter(rec.material, r, rec.p, rec.normal, scattered, temp_attenuation)) {
                attenuation *= temp_attenuation;
                r = scattered;  // Continue with the scattered ray
            } else {
                color = vec3(0.0);
                break;
            }
        } else {
            // Sky color if no hit
            vec3 unit_direction = normalize(r.direction);
            float t = 0.5 * (unit_direction.y + 1.0);
            color = attenuation * mix(vec3(1.0), vec3(0.5, 0.7, 1.0), t);
            break;
        }
        depth--;
    }

    return color;
}



void main() {
    ivec2 pixel_coord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(output_image);
    if (pixel_coord.x >= size.x || pixel_coord.y >= size.y) {
        return;
    }

    vec2 uv = vec2(pixel_coord) / vec2(image_width, image_height);

    Ray r;
    r.origin = camera_origin;

    vec2 pos = vec2(pixel_coord) / vec2(size.x - 1, size.y - 1);
    r.direction = normalize(mix(mix(ray00, ray01, pos.y), mix(ray10, ray11, pos.y), pos.x));

    vec3 color = ray_color(r);

    imageStore(output_image, pixel_coord, vec4(color, 1.0));
}
