#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>

using namespace cv;https://github.com/Wen-Binghui/Helpless_guy/blob/develop/get_direction.cpp
using namespace std;

void get_random_dis(float* rand_dis) {
    for (int i = 0; i < 3; ++i) rand_dis[i] = rand() / float(RAND_MAX);
}

void get_random_pos(float* rand_pos) {
    for (int i = 0; i < 3; ++i) rand_pos[i] = 2.0 * 3.1415926535 * rand() / float(RAND_MAX);
}

void insert_and_pop(float* mat, float* new_dis) {
    float cmat[15];
    memcpy(cmat, mat, 15 * sizeof(float));
    for (int i = 0; i < 12; ++i) {
        mat[i] = cmat[i + 3];
    }
    for (int i = 12; i < 15; ++i) {
        mat[i] = new_dis[i - 12];
    }
}

float direction_vec[30] = {
	0, 1, 0, // Forwards                       
    0.7071, 0.7071, 0, // Right-Forwards
    1, 0, 0, // Right
    0.7071, -0.7071, 0, // Right-Backwards
    0, -1, 0, // Backwards
    -0.7071, -0.7071, 0, // Left-Backwards
    -1, 0, 0, // Left
    -0.7071, 0.7071, 0, // Left-Forwards
     0, 0, 1, // Up
    0, 0, -1, // Down
};

int get_direction(float dx, float dy, float dz, float pos0, float pos1, float pos2, bool* over_speed, float speed_threshold) {
    float norm = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2)), min_dis = FLT_MAX;
    dx /= norm;
    dy /= norm;
    dz /= norm;
    //    cout << dx << " - " << dy << " - " << dz << endl;
    int dir_flag;
    int rotating_angle_in_z;
    int direction_in_pose;
    if (speed_threshold < norm) *over_speed = true;
    else *over_speed = false;
    int n_dir = sizeof(direction_vec) / sizeof(float) / 3;
    for (int i = 0; i < n_dir; ++i) {
        float cur_dis = pow(dx - direction_vec[i * 3 + 0], 2) + pow(dy - direction_vec[i * 3 + 1], 2)
            + pow(dz - direction_vec[i * 3 + 2], 2);
        if (cur_dis < min_dis) {
            dir_flag = i;
            min_dis = cur_dis;
        }
    }
    // calculating moving direction with regarding to the pose
    if (dir_flag < 8) {
        // converting angle representation
        rotating_angle_in_z = int(pos2 * 180 / 3.1415926535 + 22.5) % 360;
        // adjust direction
        direction_in_pose = (dir_flag + rotating_angle_in_z / 45) % 8;
    }
	else {
		direction_in_pose = dir_flag;
	}
    return direction_in_pose;
}

int main() {
    float h = 0.033; // time step to be defined
    float speed_threshold = 15;  // speed threshold to be defined

    bool over_speed;
    float mat_dis[15];
    float mat_pos[15];
    int iter = 0;
    int flag_direction = 0;
	int direction_in_pose;

    while (iter < 35) {
        float dis[3], pos[3]; // read
        get_random_dis(dis);
        get_random_pos(pos);
        if (iter < 5) {
            for (int i = 0; i < 3; ++i) {
                mat_dis[iter * 3 + i] = dis[i];
                mat_pos[iter * 3 + i] = pos[i];
            }

        }
        else {
            insert_and_pop(mat_dis, dis);
            insert_and_pop(mat_pos, pos);
        }
        if (iter >= 4) { // if we have 5 points, start calculating derivative
            float dx = (mat_dis[0] - 8 * mat_dis[3] + 8 * mat_dis[6] - mat_dis[9]) / 12 / h; // Four-point central difference
            float dy = (mat_dis[1] - 8 * mat_dis[4] + 8 * mat_dis[7] - mat_dis[10]) / 12 / h;
            float dz = (mat_dis[2] - 8 * mat_dis[5] + 8 * mat_dis[8] - mat_dis[11]) / 12 / h;
            float pos_0 = (mat_pos[0] + mat_pos[3] + mat_pos[6] + mat_pos[9] + mat_pos[12]) / 5.0f;
            float pos_1 = (mat_pos[1] + mat_pos[4] + mat_pos[7] + mat_pos[10] + mat_pos[13]) / 5.0f;
            float pos_2 = (mat_pos[2] + mat_pos[5] + mat_pos[8] + mat_pos[11] + mat_pos[14]) / 5.0f;
            //            cout << dx << " - " << dy << " - " << dz << endl;
            cout << pos_0 << " - " << pos_1 << " - " << pos_2 << endl;
            direction_in_pose = get_direction(dx, dy, dz, pos_0, pos_1, pos_2, &over_speed, speed_threshold);
            cout << "is_overspeed: " << over_speed << endl;
            cout << "direction_id: " << direction_in_pose << endl;
        }

        iter++;
    }

    return 0;
}
