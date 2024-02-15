
typedef struct {
    float x, y, vx, vy;
} Boid;

__kernel void update_boids(__global Boid* boids, const int num_boids, const float dt, const float width, const float height) {
    int index = get_global_id(0);
    if (index < num_boids) {
        // Update boid position and velocity based on simple rules
        float coh_radius = 200.0f;
        float align_radius = 200.0f;
        float sep_radius = 10.0f;
        float max_speed = 200.0f;
        float max_force = 0.1f;

        float coh_factor = 0.01f;
        float align_factor = 0.05f;
        float sep_factor = 0.5f;

        float coh_x = 0.0f, coh_y = 0.0f;
        float align_x = 0.0f, align_y = 0.0f;
        float sep_x = 0.0f, sep_y = 0.0f;
        int coh_count = 0, align_count = 0, sep_count = 0;

        // Loop through neighboring boids
        for (int i = 0; i < num_boids; ++i) {

            if (boids[i].x > width) boids[i].x = 0.0f;
            if (boids[i].x < 0.0f) boids[i].x = width;
            if (boids[i].y > height) boids[i].y = 0.0f;
            if (boids[i].y < 0.0f)  boids[i].y = height;

            if (i != index) {
                float dx = boids[i].x - boids[index].x;
                float dy = boids[i].y - boids[index].y;
                
                float dist_sq = dx * dx + dy * dy;

                if (dist_sq < coh_radius * coh_radius) {
                    coh_x += boids[i].x;
                    coh_y += boids[i].y;
                    coh_count++;
                }

                if (dist_sq < align_radius * align_radius) {
                    align_x += boids[i].vx;
                    align_y += boids[i].vy;
                    align_count++;
                }

                if (dist_sq < sep_radius * sep_radius) {
                    sep_x -= dx / dist_sq;
                    sep_y -= dy / dist_sq;
                    sep_count++;
                }
            }
        }

        // Apply cohesion rule
        if (coh_count > 0) {
            coh_x /= coh_count;
            coh_y /= coh_count;

            float coh_force_x = (coh_x - boids[index].x) * coh_factor;
            float coh_force_y = (coh_y - boids[index].y) * coh_factor;

            boids[index].vx += coh_force_x;
            boids[index].vy += coh_force_y;
        }

        // Apply alignment rule
        if (align_count > 0) {
            align_x /= align_count;
            align_y /= align_count;

            float align_force_x = (align_x - boids[index].vx) * align_factor;
            float align_force_y = (align_y - boids[index].vy) * align_factor;

            boids[index].vx += align_force_x;
            boids[index].vy += align_force_y;
        }

        // Apply separation rule
        if (sep_count > 0) {
            float sep_force_x = sep_x * sep_factor;
            float sep_force_y = sep_y * sep_factor;

            boids[index].vx += sep_force_x;
            boids[index].vy += sep_force_y;
        }

        // Limit speed
        float speed_sq = boids[index].vx * boids[index].vx + boids[index].vy * boids[index].vy;
        if (speed_sq > max_speed * max_speed) {
            float scale = max_speed / sqrt(speed_sq);
            boids[index].vx *= scale;
            boids[index].vy *= scale;
        }

        // Update position
        boids[index].x += boids[index].vx * dt;
        boids[index].y += boids[index].vy * dt;
    }
}
