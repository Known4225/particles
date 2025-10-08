// #define TURTLE_IMPLEMENTATION
#include "turtle.h"
#include <time.h>

typedef enum {
    PARTICLE_TYPE_ELECTRON = 0,
    PARTICLE_TYPE_NUCLEUS,
    NUMBER_OF_PARTICLE_TYPES,
} particle_type_t;

/* names of particles */
char particle_type_ascii[][128] = {
    "Electron",
    "Nucleus",
    "Invalid",
};

typedef enum {
    PI_TYPE = 0,
    PI_CLUSTER = 1,
    PI_XPOS = 2,
    PI_YPOS = 3,
    PI_XVEL = 4,
    PI_YVEL = 5,
    PI_SIZE = 6,
    PI_DIR = 7,
    PI_DIRECTION = 7,
    PI_DIRCHANGE = 8,
    PI_DIRECTIONCHANGE = 8,
    PI_DIRECTION_CHANGE = 8,
    PI_NUMBER_OF_FIELDS = 20,
} particle_index_t;

typedef struct {
    list_t *particles; // type, cluster, xpos, ypos, xvel, yvel, size, direction, direction change, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved
    list_t *massTable;
    list_t *diameterTable;
    int32_t electronNucleusBoundary;
    int8_t pause;
    int8_t oneTick;
    int8_t drawRadius;
    char keys[8];
    double simulationSpeed;
    double heat;
    tt_slider_t *heatSlider;
} particles_t;

particles_t self;

void printParticle(int32_t index);

void createParticle(particle_type_t type, int32_t cluster, double xpos, double ypos, double xvel, double yvel, double size, double direction, double dirchange) {
    list_append(self.particles, (unitype) type, 'i'); // type
    list_append(self.particles, (unitype) cluster, 'i'); // cluster
    list_append(self.particles, (unitype) xpos, 'd'); // xpos
    list_append(self.particles, (unitype) ypos, 'd'); // ypos
    list_append(self.particles, (unitype) xvel, 'd'); // xvel
    list_append(self.particles, (unitype) yvel, 'd'); // yvel
    list_append(self.particles, (unitype) size, 'd'); // size
    list_append(self.particles, (unitype) direction, 'd'); // direction
    list_append(self.particles, (unitype) dirchange, 'd'); // direction change
    for (int32_t j = 0; j < 11; j++) {
        list_append(self.particles, (unitype) 0, 'i');
    }
}

void init() {
    self.electronNucleusBoundary = 2; // cluster of 2 is an electron, cluster of 3 is a nucleus
    self.pause = 0;
    self.oneTick = 0;
    self.drawRadius = 0;
    self.simulationSpeed = 1;
    self.heat = 1000;
    self.heatSlider = sliderInit("Heat", &self.heat, TT_SLIDER_TYPE_VERTICAL, TT_SLIDER_ALIGN_CENTER, 300, 100, 10, 80, 0, 80000, 1);
    self.heatSlider -> scale = TT_SLIDER_SCALE_EXP;
    self.massTable = list_init();
    for (int32_t i = 0; i < 100; i++) {
        list_append(self.massTable, (unitype) (double) i, 'd');
    }
    self.diameterTable = list_init();
    list_append(self.diameterTable, (unitype) 0.0, 'd');
    list_append(self.diameterTable, (unitype) 1.0, 'd');
    list_append(self.diameterTable, (unitype) 4.0, 'd');
    for (int32_t i = 0; i < 100; i++) {
        list_append(self.diameterTable, (unitype) 9.0, 'd');
    }
    srand(time(NULL));
    self.particles = list_init();
    /* randomly generate particles */
    int32_t startingParticles = 100; // number of particles
    for (int32_t i = 0; i < startingParticles; i++) {
        int32_t type = randomInt(0, NUMBER_OF_PARTICLE_TYPES - 1);
        list_append(self.particles, (unitype) type, 'i'); // type
        if (type == PARTICLE_TYPE_ELECTRON) {
            list_append(self.particles, (unitype) randomInt(1, self.electronNucleusBoundary), 'i'); // cluster (1 or 2)
        } else if (type == PARTICLE_TYPE_NUCLEUS) {
            list_append(self.particles, (unitype) randomInt(self.electronNucleusBoundary + 1, 5), 'i'); // cluster (3 to 5)
        } else {
            list_append(self.particles, (unitype) 0, 'i'); // cluster (0)
        }
        list_append(self.particles, (unitype) randomDouble(-320, 320), 'd'); // xpos
        list_append(self.particles, (unitype) randomDouble(-180, 180), 'd'); // ypos
        list_append(self.particles, (unitype) randomDouble(-3, 3), 'd'); // xvel
        list_append(self.particles, (unitype) randomDouble(-3, 3), 'd'); // yvel
        list_append(self.particles, (unitype) 1.0, 'd'); // size
        list_append(self.particles, (unitype) randomDouble(0, 360), 'd'); // direction
        list_append(self.particles, (unitype) randomDouble(-5, 5), 'd'); // direction change
        for (int32_t j = 0; j < 11; j++) {
            list_append(self.particles, (unitype) 0, 'i');
        }
        // printParticle(i * 20);
    }
    int32_t type = PARTICLE_TYPE_NUCLEUS;
    list_append(self.particles, (unitype) type, 'i'); // typef 
    if (type == PARTICLE_TYPE_ELECTRON) {
        list_append(self.particles, (unitype) randomInt(1, self.electronNucleusBoundary), 'i'); // cluster (1 or 2)
    } else if (type == PARTICLE_TYPE_NUCLEUS) {
        list_append(self.particles, (unitype) randomInt(self.electronNucleusBoundary + 1, 5), 'i'); // cluster (3 to 5)
    } else {
        list_append(self.particles, (unitype) 0, 'i'); // cluster (0)
    }
    list_append(self.particles, (unitype) -150.0, 'd'); // xpos
    list_append(self.particles, (unitype) (-4.5 * sqrt(3) / 2), 'd'); // ypos
    list_append(self.particles, (unitype) 1.0, 'd'); // xvel
    list_append(self.particles, (unitype) 0, 'd'); // yvel
    list_append(self.particles, (unitype) 1.0, 'd'); // size
    list_append(self.particles, (unitype) randomDouble(0, 360), 'd'); // direction
    list_append(self.particles, (unitype) randomDouble(-5, 5), 'd'); // direction change
    for (int32_t j = 0; j < 11; j++) {
        list_append(self.particles, (unitype) 0, 'i');
    }
    type = PARTICLE_TYPE_NUCLEUS;
    list_append(self.particles, (unitype) type, 'i'); // type
    if (type == PARTICLE_TYPE_ELECTRON) {
        list_append(self.particles, (unitype) randomInt(1, self.electronNucleusBoundary), 'i'); // cluster (1 or 2)
    } else if (type == PARTICLE_TYPE_NUCLEUS) {
        list_append(self.particles, (unitype) randomInt(self.electronNucleusBoundary + 1, 5), 'i'); // cluster (3 to 5)
    } else {
        list_append(self.particles, (unitype) 0, 'i'); // cluster (0)
    }
    list_append(self.particles, (unitype) 150.0, 'd'); // xpos
    list_append(self.particles, (unitype) (-4.5 * sqrt(3) / 2), 'd'); // ypos
    list_append(self.particles, (unitype) -1.0, 'd'); // xvel
    list_append(self.particles, (unitype) 0, 'd'); // yvel
    list_append(self.particles, (unitype) 1.0, 'd'); // size
    list_append(self.particles, (unitype) randomDouble(0, 360), 'd'); // direction
    list_append(self.particles, (unitype) randomDouble(-5, 5), 'd'); // direction change
    for (int32_t j = 0; j < 11; j++) {
        list_append(self.particles, (unitype) 0, 'i');
    }
    type = PARTICLE_TYPE_NUCLEUS;
    list_append(self.particles, (unitype) type, 'i'); // type
    if (type == PARTICLE_TYPE_ELECTRON) {
        list_append(self.particles, (unitype) randomInt(1, self.electronNucleusBoundary), 'i'); // cluster (1 or 2)
    } else if (type == PARTICLE_TYPE_NUCLEUS) {
        list_append(self.particles, (unitype) randomInt(self.electronNucleusBoundary + 1, 5), 'i'); // cluster (3 to 5)
    } else {
        list_append(self.particles, (unitype) 0, 'i'); // cluster (0)
    }
    list_append(self.particles, (unitype) 0, 'd'); // xpos
    list_append(self.particles, (unitype) 149.8, 'd'); // ypos
    list_append(self.particles, (unitype) 0.0, 'd'); // xvel
    list_append(self.particles, (unitype) -1.0, 'd'); // yvel
    list_append(self.particles, (unitype) 1.0, 'd'); // size
    list_append(self.particles, (unitype) randomDouble(0, 360), 'd'); // direction
    list_append(self.particles, (unitype) randomDouble(-5, 5), 'd'); // direction change
    for (int32_t j = 0; j < 11; j++) {
        list_append(self.particles, (unitype) 0, 'i');
    }
}

/* print particle attributes */
void printParticle(int32_t index) {
    printf("Particle %d (%s)\n", index, particle_type_ascii[self.particles -> data[index + PI_TYPE].i]);
    printf("- cluster: %d\n", self.particles -> data[index + PI_CLUSTER].i);
    printf("- xpos: %lf\n", self.particles -> data[index + PI_XPOS].d);
    printf("- ypos: %lf\n", self.particles -> data[index + PI_YPOS].d);
    printf("- xvel: %lf\n", self.particles -> data[index + PI_XVEL].d);
    printf("- yvel: %lf\n", self.particles -> data[index + PI_YVEL].d);
    printf("- size: %lf\n", self.particles -> data[index + PI_SIZE].d);
    printf("- direction: %lf\n", self.particles -> data[index + PI_DIR].d);
    printf("- direction change: %lf\n", self.particles -> data[index + PI_DIRCHANGE].d);
}

/* draw a particle */
void renderParticle(int32_t index) {
    switch (self.particles -> data[index + PI_TYPE].i) {
        case PARTICLE_TYPE_ELECTRON:
        turtlePenSize(self.particles -> data[index + PI_SIZE].d * 2);
        tt_setColor(TT_COLOR_RED);
        int32_t clusterE = self.particles -> data[index + PI_CLUSTER].i;
        if (clusterE == 1) {
            turtleGoto(self.particles -> data[index + PI_XPOS].d, self.particles -> data[index + PI_YPOS].d);
            turtlePenDown();
            turtlePenUp();
        } else {
            double theta = self.particles -> data[index + PI_DIRECTION].d / 57.2958;
            for (int32_t i = 0; i < clusterE; i++) {
                turtleGoto(self.particles -> data[index + PI_XPOS].d + sin(theta) * self.particles -> data[index + PI_SIZE].d * 0.7, self.particles -> data[index + PI_YPOS].d + cos(theta) * self.particles -> data[index + PI_SIZE].d * 0.7);
                turtlePenDown();
                turtlePenUp();
                theta += 360.0 / clusterE / 57.2958;
            }
        }
        break;
        case PARTICLE_TYPE_NUCLEUS:
        turtlePenSize(self.particles -> data[index + PI_SIZE].d * 5);
        tt_setColor(TT_COLOR_LIGHT_GREY);
        double theta = self.particles -> data[index + PI_DIRECTION].d / 57.2958;
        int32_t clusterN = self.particles -> data[index + PI_CLUSTER].i;
        for (int32_t i = 0; i < clusterN; i++) {
            turtleGoto(self.particles -> data[index + PI_XPOS].d + sin(theta) * self.particles -> data[index + PI_SIZE].d * 2, self.particles -> data[index + PI_YPOS].d + cos(theta) * self.particles -> data[index + PI_SIZE].d * 2);
            turtlePenDown();
            turtlePenUp();
            theta += 360.0 / clusterN / 57.2958;
        }
        break;
    }
    if (self.drawRadius) {
        turtlePenSize(self.particles -> data[index + PI_SIZE].d * self.diameterTable -> data[self.particles -> data[index + PI_CLUSTER].i].d);
        turtleGoto(self.particles -> data[index + PI_XPOS].d, self.particles -> data[index + PI_YPOS].d);
        turtlePenDown();
        turtlePenUp();
    }
}

void render() {
    /* render particles */
    for (int32_t i = 0; i < self.particles -> length; i += PI_NUMBER_OF_FIELDS) {
        renderParticle(i);
    }
    if (self.pause && !self.oneTick) {
        return;
    }
    for (int32_t i = 0; i < self.particles -> length; i += PI_NUMBER_OF_FIELDS) {
        /* check collision (particles need to check if they've crossed paths with any other particle within the circle with a radius of their velocity vector) */
        double centerX = 0;
        double Ar = self.particles -> data[i + PI_SIZE].d * self.diameterTable -> data[self.particles -> data[i + PI_CLUSTER].i].d / 2;
        double Ax = self.particles -> data[i + PI_XPOS].d;
        double Ay = self.particles -> data[i + PI_YPOS].d;
        double Avx = self.particles -> data[i + PI_XVEL].d;
        double Avy = self.particles -> data[i + PI_YVEL].d;
        for (int32_t j = 0; j < i; j += PI_NUMBER_OF_FIELDS) {
            double Br = self.particles -> data[j + PI_SIZE].d * self.diameterTable -> data[self.particles -> data[j + PI_CLUSTER].i].d / 2;
            double Bx = self.particles -> data[j + PI_XPOS].d;
            double By = self.particles -> data[j + PI_YPOS].d;
            double Bvx = self.particles -> data[j + PI_XVEL].d;
            double Bvy = self.particles -> data[j + PI_YVEL].d;
            double discriminant = (2 * (Avx - Bvx) * (Ax - Bx) + 2 * (Avy - Bvy) * (Ay - By)) * (2 * (Avx - Bvx) * (Ax - Bx) + 2 * (Avy - Bvy) * (Ay - By)) + 4 * ((Avx - Bvx) * (Avx - Bvx) + (Avy - Bvy) * (Avy - Bvy)) * ((Ar + Br) * (Ar + Br) - Ax * Ax + 2 * (Ax * Bx) - Bx * Bx - Ay * Ay + 2 * (Ay * By) - By * By);
            if (discriminant <= 0) {
                /* no collision */
            } else {
                double sqrtDisc = sqrt(discriminant);
                double t1 = (-2 * (Avx - Bvx) * (Ax - Bx) - 2 * (Avy - Bvy) * (Ay - By) + sqrtDisc) / (2 * ((Avx - Bvx) * (Avx - Bvx) + (Avy - Bvy) * (Avy - Bvy)));
                double t2 = (-2 * (Avx - Bvx) * (Ax - Bx) - 2 * (Avy - Bvy) * (Ay - By) - sqrtDisc) / (2 * ((Avx - Bvx) * (Avx - Bvx) + (Avy - Bvy) * (Avy - Bvy)));
                if (t2 > 0 && t1 > 0 && (t1 <= self.simulationSpeed || t2 <= self.simulationSpeed)) {
                    if (t1 > t2) {
                        t1 = t2;
                    }
                    double oldXi = self.particles -> data[i + PI_XVEL].d;
                    double oldYi = self.particles -> data[i + PI_YVEL].d;
                    double oldXj = self.particles -> data[j + PI_XVEL].d;
                    double oldYj = self.particles -> data[j + PI_YVEL].d;
                    double massi = self.massTable -> data[self.particles -> data[i + PI_CLUSTER].i].d;
                    double massj = self.massTable -> data[self.particles -> data[j + PI_CLUSTER].i].d;
                    if (self.particles -> data[i + PI_TYPE].i == self.particles -> data[j + PI_TYPE].i) {
                        /* update j */
                        self.particles -> data[j + PI_XVEL].d = (massi * oldXi + massj * oldXj) / (massi + massj);
                        self.particles -> data[j + PI_YVEL].d = (massi * oldYi + massj * oldYj) / (massi + massj);
                        self.particles -> data[j + PI_CLUSTER].i += self.particles -> data[i + PI_CLUSTER].i;
                        if (self.particles -> data[j + PI_CLUSTER].i <= 2) {
                            self.particles -> data[j + PI_TYPE].i = PARTICLE_TYPE_ELECTRON;
                        } else {
                            self.particles -> data[j + PI_TYPE].i = PARTICLE_TYPE_NUCLEUS;
                        }
                        /* delete i */
                        list_delete_range(self.particles, i, i + PI_NUMBER_OF_FIELDS);
                        i -= PI_NUMBER_OF_FIELDS;
                        /* simulate time */
                        self.particles -> data[j + PI_XPOS].d += oldXj * t1 - self.particles -> data[j + PI_XVEL].d * t1;
                        self.particles -> data[j + PI_YPOS].d += oldYj * t1 - self.particles -> data[j + PI_YVEL].d * t1;
                    } else if (self.particles -> data[i + PI_TYPE].i != self.particles -> data[j + PI_TYPE].i) {
                        /* simulate time (to contact) */
                        self.particles -> data[i + PI_XPOS].d += oldXi * t1;
                        self.particles -> data[i + PI_YPOS].d += oldYi * t1;
                        self.particles -> data[j + PI_XPOS].d += oldXj * t1;
                        self.particles -> data[j + PI_YPOS].d += oldYj * t1;
                        double commonSpeed = (massi * sqrt(oldXi * oldXi + oldYi * oldYi) + massj * sqrt(oldXj * oldXj + oldYj * oldYj)) / (self.particles -> data[i + PI_CLUSTER].i + self.particles -> data[j + PI_CLUSTER].i);
                        /* update j */
                        double theta = self.particles -> data[j + PI_DIR].d / 57.2958;
                        for (int32_t k = 0; k < self.particles -> data[j + PI_CLUSTER].i - 1; k++) {
                            theta += 360.0 / self.particles -> data[j + PI_CLUSTER].i / 57.2958;
                            createParticle(PARTICLE_TYPE_ELECTRON, 1, self.particles -> data[j + PI_XPOS].d, self.particles -> data[j + PI_YPOS].d, cos(theta) * commonSpeed, sin(theta) * commonSpeed, 1.0, randomDouble(0, 360), randomDouble(-commonSpeed * 2, commonSpeed * 2));
                            self.particles -> data[self.particles -> length - PI_NUMBER_OF_FIELDS + PI_XPOS].d -= self.particles -> data[j + PI_XVEL].d * t1;
                            self.particles -> data[self.particles -> length - PI_NUMBER_OF_FIELDS + PI_YPOS].d -= self.particles -> data[j + PI_YVEL].d * t1;
                        }
                        theta = self.particles -> data[j + PI_DIR].d / 57.2958;
                        self.particles -> data[j + PI_TYPE].i = PARTICLE_TYPE_ELECTRON;
                        self.particles -> data[j + PI_CLUSTER].i = 1;
                        self.particles -> data[j + PI_XVEL].d = cos(theta) * commonSpeed;
                        self.particles -> data[j + PI_YVEL].d = sin(theta) * commonSpeed;
                        self.particles -> data[j + PI_DIR].d = randomDouble(0, 360);
                        self.particles -> data[j + PI_DIRCHANGE].d =  randomDouble(-commonSpeed * 2, commonSpeed * 2);
                        self.particles -> data[j + PI_XPOS].d -= self.particles -> data[j + PI_XVEL].d * t1;
                        self.particles -> data[j + PI_YPOS].d -= self.particles -> data[j + PI_YVEL].d * t1;
                        /* update i */
                        theta = self.particles -> data[i + PI_DIR].d / 57.2958;
                        for (int32_t k = 0; k < self.particles -> data[i + PI_CLUSTER].i - 1; k++) {
                            theta += 360.0 / self.particles -> data[i + PI_CLUSTER].i / 57.2958;
                            createParticle(PARTICLE_TYPE_ELECTRON, 1, self.particles -> data[i + PI_XPOS].d, self.particles -> data[i + PI_YPOS].d, cos(theta) * commonSpeed, sin(theta) * commonSpeed, 1.0, randomDouble(0, 360), randomDouble(-commonSpeed * 2, commonSpeed * 2));
                            self.particles -> data[self.particles -> length - PI_NUMBER_OF_FIELDS + PI_XPOS].d -= self.particles -> data[i + PI_XVEL].d * t1;
                            self.particles -> data[self.particles -> length - PI_NUMBER_OF_FIELDS + PI_YPOS].d -= self.particles -> data[i + PI_YVEL].d * t1;
                        }
                        theta = self.particles -> data[i + PI_DIR].d / 57.2958;
                        self.particles -> data[i + PI_TYPE].i = PARTICLE_TYPE_ELECTRON;
                        self.particles -> data[i + PI_CLUSTER].i = 1;
                        self.particles -> data[i + PI_XVEL].d = cos(theta) * commonSpeed;
                        self.particles -> data[i + PI_YVEL].d = sin(theta) * commonSpeed;
                        self.particles -> data[i + PI_DIR].d = randomDouble(0, 360);
                        self.particles -> data[i + PI_DIRCHANGE].d =  randomDouble(-commonSpeed * 2, commonSpeed * 2);
                        self.particles -> data[i + PI_XPOS].d -= self.particles -> data[i + PI_XVEL].d * t1;
                        self.particles -> data[i + PI_YPOS].d -= self.particles -> data[i + PI_YVEL].d * t1;
                    } else {
                        self.particles -> data[i + PI_XVEL].d = (oldXi * (massi - massj) + 2 * massj * oldXj) / (massi + massj);
                        self.particles -> data[i + PI_YVEL].d = (oldYi * (massi - massj) + 2 * massj * oldYj) / (massi + massj);
                        self.particles -> data[j + PI_XVEL].d = oldXi - oldXj + self.particles -> data[i + PI_XVEL].d;
                        self.particles -> data[j + PI_YVEL].d = oldYi - oldYj + self.particles -> data[i + PI_YVEL].d;
                        /* simulate time */
                        self.particles -> data[i + PI_XPOS].d += oldXi * t1 - self.particles -> data[i + PI_XVEL].d * t1;
                        self.particles -> data[i + PI_YPOS].d += oldYi * t1 - self.particles -> data[i + PI_YVEL].d * t1;
                        self.particles -> data[j + PI_XPOS].d += oldXj * t1 - self.particles -> data[j + PI_XVEL].d * t1;
                        self.particles -> data[j + PI_YPOS].d += oldYj * t1 - self.particles -> data[j + PI_YVEL].d * t1;
                    }
                }
            }
        }
    }
    for (int32_t i = 0; i < self.particles -> length; i += PI_NUMBER_OF_FIELDS) {
        /* move particles */
        self.particles -> data[i + PI_XPOS].d += self.particles -> data[i + PI_XVEL].d * self.simulationSpeed;
        if (self.particles -> data[i + PI_XPOS].d >= 320) {
            self.particles -> data[i + PI_XPOS].d -= 640;
        }
        if (self.particles -> data[i + PI_XPOS].d < -320) {
            self.particles -> data[i + PI_XPOS].d += 640;
        }
        self.particles -> data[i + PI_YPOS].d += self.particles -> data[i + PI_YVEL].d * self.simulationSpeed;
        while (self.particles -> data[i + PI_YPOS].d >= 180) {
            self.particles -> data[i + PI_YPOS].d -= 360;
        }
        while (self.particles -> data[i + PI_YPOS].d < -180) {
            self.particles -> data[i + PI_YPOS].d += 360;
        }
        self.particles -> data[i + PI_DIRECTION].d += self.particles -> data[i + PI_DIRCHANGE].d * self.simulationSpeed;
        while (self.particles -> data[i + PI_DIRECTION].d >= 360) {
            self.particles -> data[i + PI_DIRECTION].d -= 360;
        }
        while (self.particles -> data[i + PI_DIRECTION].d < 0) {
            self.particles -> data[i + PI_DIRECTION].d += 360;
        }
    }
    self.oneTick = 0;
}

void renderUI() {
    tt_setColor(TT_COLOR_BLACK);
    turtle.pena = 0.85;
    turtleRectangle(280, -180, 320, 180);
    tt_setColor(TT_COLOR_TEXT);
    turtleTextWriteStringf(314, -170, 10, 100, "%d", self.particles -> length / PI_NUMBER_OF_FIELDS);
}

void mouseTick() {
    if (turtleKeyPressed(GLFW_KEY_SPACE)) {
        if (self.keys[3] == 0) {
            self.keys[3] = 1;
            self.pause = !self.pause;
        }
    } else {
        self.keys[3] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_R)) {
        if (self.keys[4] == 0) {
            self.keys[4] = 1;
            self.drawRadius = !self.drawRadius;
        }
    } else {
        self.keys[4] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_F)) {
        if (self.keys[5] == 0 || self.keys[5] == 1) {
            if (self.keys[5] == 0) {
                self.keys[5] = 70;
            } else {
                self.keys[5] = 4;
            }
            if (self.pause) {
                self.oneTick = 1;
            }
        } else {
            self.keys[5]--;
        }
    } else {
        self.keys[5] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_UP)) {
        if (self.keys[6] == 0 || self.keys[6] == 1) {
            if (self.keys[6] == 0) {
                self.keys[6] = 70;
            } else {
                self.keys[6] = 4;
            }
            self.heat *= 1.05;
            if (self.heat > 10000) {
                self.heat = 10000;
            }
        } else {
            self.keys[6]--;
        }
    } else {
        self.keys[6] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_DOWN)) {
        if (self.keys[7] == 0 || self.keys[7] == 1) {
            if (self.keys[7] == 0) {
                self.keys[7] = 70;
            } else {
                self.keys[7] = 4;
            }
            self.heat /= 1.05;
        } else {
            self.keys[7]--;
        }
    } else {
        self.keys[7] = 0;
    }
    self.simulationSpeed = self.heat / 1000;
}

void parseRibbonOutput() {
    if (tt_ribbon.output[0] == 0) {
        return;
    }
    tt_ribbon.output[0] = 0;
    if (tt_ribbon.output[1] == 0) { // File
        if (tt_ribbon.output[2] == 1) { // New
            printf("New\n");
        }
        if (tt_ribbon.output[2] == 2) { // Save
            if (osToolsFileDialog.selectedFilenames -> length == 0) {
                if (osToolsFileDialogSave(OSTOOLS_FILE_DIALOG_FILE, "", NULL) != -1) {
                    printf("Saved to: %s\n", osToolsFileDialog.selectedFilenames -> data[0].s);
                }
            } else {
                printf("Saved to: %s\n", osToolsFileDialog.selectedFilenames -> data[0].s);
            }
        }
        if (tt_ribbon.output[2] == 3) { // Save As...
            if (osToolsFileDialogSave(OSTOOLS_FILE_DIALOG_FILE, "", NULL) != -1) {
                printf("Saved to: %s\n", osToolsFileDialog.selectedFilenames -> data[0].s);
            }
        }
        if (tt_ribbon.output[2] == 4) { // Open
            if (osToolsFileDialogOpen(OSTOOLS_FILE_DIALOG_MULTIPLE_SELECT, OSTOOLS_FILE_DIALOG_FILE, "", NULL) != -1) {
                printf("Loaded data from: ");
                list_print(osToolsFileDialog.selectedFilenames);
            }
        }
    }
    if (tt_ribbon.output[1] == 1) { // Edit
        if (tt_ribbon.output[2] == 1) { // Undo
            printf("Undo\n");
        }
        if (tt_ribbon.output[2] == 2) { // Redo
            printf("Redo\n");
        }
        if (tt_ribbon.output[2] == 3) { // Cut
            osToolsClipboardSetText("test123");
            printf("Cut \"test123\" to clipboard!\n");
        }
        if (tt_ribbon.output[2] == 4) { // Copy
            osToolsClipboardSetText("test345");
            printf("Copied \"test345\" to clipboard!\n");
        }
        if (tt_ribbon.output[2] == 5) { // Paste
            osToolsClipboardGetText();
            printf("Pasted \"%s\" from clipboard!\n", osToolsClipboard.text);
        }
    }
    if (tt_ribbon.output[1] == 2) { // View
        if (tt_ribbon.output[2] == 1) { // Change theme
            printf("Change theme\n");
            if (tt_theme == TT_THEME_DARK) {
                turtleToolsSetTheme(TT_THEME_COLT);
            } else if (tt_theme == TT_THEME_COLT) {
                turtleToolsSetTheme(TT_THEME_NAVY);
            } else if (tt_theme == TT_THEME_NAVY) {
                turtleToolsSetTheme(TT_THEME_LIGHT);
            } else if (tt_theme == TT_THEME_LIGHT) {
                turtleToolsSetTheme(TT_THEME_DARK);
            }
        } 
        if (tt_ribbon.output[2] == 2) { // GLFW
            printf("GLFW settings\n");
        } 
    }
}

int main(int argc, char *argv[]) {
    /* Initialise glfw */
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA (Anti-Aliasing) with 4 samples (must be done before window is created (?))

    /* Create a windowed mode window and its OpenGL context */
    const GLFWvidmode *monitorSize = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int32_t windowHeight = monitorSize -> height;
    GLFWwindow *window = glfwCreateWindow(windowHeight * 16 / 9, windowHeight, "particles", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, windowHeight * 16 / 9 * 0.4, windowHeight * 0.4, windowHeight * 16 / 9, windowHeight);

    /* initialise turtle */
    turtleInit(window, -320, -180, 320, 180);
    glfwSetWindowSize(window, windowHeight * 16 / 9 * 0.85, monitorSize -> height * 0.85); // doing it this way ensures the window spawns in the top left of the monitor and fixes resizing limits
    /* initialise turtleText */
    turtleTextInit("config/roberto.tgl");
    /* initialise turtleTools ribbon */
    turtleToolsSetTheme(TT_THEME_DARK); // dark theme preset
    ribbonInit("config/ribbonConfig.txt");
    /* initialise osTools */
    osToolsInit(argv[0], window); // must include argv[0] to get executableFilepath, must include GLFW window
    osToolsFileDialogAddGlobalExtension("txt"); // add txt to extension restrictions

    init();

    uint32_t tps = 120; // ticks per second (locked to fps in this case)
    uint64_t tick = 0; // count number of ticks since application started
    clock_t start, end;

    while (turtle.close == 0) {
        start = clock();
        turtleGetMouseCoords();
        turtleClear();
        render();
        renderUI();
        mouseTick();
        turtleToolsUpdate(); // update turtleTools
        if (self.pause) {
            tt_setColor(TT_COLOR_ORANGE);
            turtleRectangle(312, 172, 318, 178);
        }
        parseRibbonOutput(); // user defined function to use ribbon
        turtleUpdate(); // update the screen
        end = clock();
        while ((double) (end - start) / CLOCKS_PER_SEC < (1.0 / tps)) {
            end = clock();
        }
        tick++;
    }
    turtleFree();
    glfwTerminate();
    return 0;
}