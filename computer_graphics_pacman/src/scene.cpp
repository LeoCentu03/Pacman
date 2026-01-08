#include "scene.h"
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Vec3 Scene::getAxisAligned(Vec3 dir) {
    if (dir.lengthSq() < 0.01f) return Vec3(1, 0, 0);
    if (std::abs(dir.x) > std::abs(dir.z)) {
        dir.z = 0;
        dir.x = (dir.x > 0) ? 1.0f : -1.0f;
    } else {
        dir.x = 0;
        dir.z = (dir.z > 0) ? 1.0f : -1.0f;
    }
    return dir.normalized();
}

Vec3 Scene::getRandomDirection() {
      float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * M_PI;
      Vec3 dir = Vec3(cos(angle), 0, sin(angle));
      return getAxisAligned(dir);
}

Vec3 Scene::getTurnedLeft(const Vec3& dir) {
    if (dir.lengthSq() < 0.01f) return getRandomDirection();
    return Vec3(-dir.z, 0, dir.x).normalized();
}

Vec3 Scene::getTurnedRight(const Vec3& dir) {
      if (dir.lengthSq() < 0.01f) return getRandomDirection();
    return Vec3(dir.z, 0, -dir.x).normalized();
}

void drawGround() {
    glColor3f(0.05f, 0.05f, 0.1f); 
    float groundSize = 500.0f;
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-groundSize, 0.0f, -groundSize);
    glVertex3f( groundSize, 0.0f, -groundSize);
    glVertex3f( groundSize, 0.0f,  groundSize);
    glVertex3f(-groundSize, 0.0f,  groundSize);
    glEnd();
    glColor3f(0.1f, 0.1f, 0.2f);
    glBegin(GL_LINES);
    for (float i = -groundSize; i <= groundSize; i += 5.0f) {
        glVertex3f(i, 0.01f, -groundSize);
        glVertex3f(i, 0.01f, groundSize);
        glVertex3f(-groundSize, 0.01f, i);
        glVertex3f(groundSize, 0.01f, i);
    }
    glEnd();
}

void drawSimpleMaze(const std::vector<Wall>& walls) {
    glColor3f(0.05f, 0.1f, 0.4f); 
    float wallHeight = 2.0f;
    for (const auto& wall : walls) {
        float cx = (wall.x_min + wall.x_max) / 2.0f;
        float cz = (wall.z_min + wall.z_max) / 2.0f;
        float width = wall.x_max - wall.x_min;
        float depth = wall.z_max - wall.z_min;
        glPushMatrix();
        glTranslatef(cx, wallHeight / 2.0f, cz);
        glScalef(width, wallHeight, depth);
        glBegin(GL_QUADS);
        glNormal3f(0,0,1); glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glNormal3f(0,0,-1); glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
        glNormal3f(0,1,0); glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
        glNormal3f(0,-1,0); glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
        glNormal3f(1,0,0); glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
        glNormal3f(-1,0,0); glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
        glEnd();
        glPopMatrix();
    }
}

void Scene::init() {
    srand(static_cast<unsigned int>(time(NULL)));
    pacman.init();
    pacman.position = Vec3(-13.0f, 1.0f, 13.0f); // Pacman parte dall'alto (z=13.0)
    pacman.startPosition = pacman.position; // Importante per la logica di CHASE_PELLET

    ghosts.clear();
    Ghost g1, g2, g3;
    
    float far_x = 13.0f;
    float far_z = 13.0f;
    float mid_x = 0.0f;
    float mid_z = 0.0f;

    // Fantasmi spostati nel quadrante opposto a Pacman per mantenere la distanza
    g1.init(far_x, -far_z, 1.0f, 0.2f, 0.2f); 
    g2.init(far_x, mid_z, 0.2f, 1.0f, 1.0f); 
    g3.init(mid_x, -far_z, 1.0f, 0.7f, 0.2f); 
    
    ghosts.push_back(g1);
    ghosts.push_back(g2);
    ghosts.push_back(g3);

    powerPellet.init(0.0f, 0.0f);
    totalTime = 0.0;
    currentState = CHASING_PELLET;
    targetGhost = nullptr;

    walls.clear();
    float maze_bound = 16.5f;
    float wall_thickness = 1.0f; 
    float gap_size = 3.0f;
    float inner_wall_x = 8.0f;
    float inner_wall_z = 4.0f;
    float inner_wall_thickness = 0.1f; 

    walls.push_back({-maze_bound, maze_bound, maze_bound - wall_thickness, maze_bound});
    walls.push_back({-maze_bound, maze_bound, -maze_bound, -maze_bound + wall_thickness});
    walls.push_back({maze_bound - wall_thickness, maze_bound, -maze_bound + wall_thickness, maze_bound - wall_thickness});
    walls.push_back({-maze_bound, -maze_bound + wall_thickness, -maze_bound + wall_thickness, maze_bound - wall_thickness});

    walls.push_back({-inner_wall_x, -gap_size/2.0f, inner_wall_z - inner_wall_thickness/2.0f, inner_wall_z + inner_wall_thickness/2.0f});
    walls.push_back({gap_size/2.0f, inner_wall_x, inner_wall_z - inner_wall_thickness/2.0f, inner_wall_z + inner_wall_thickness/2.0f});
    walls.push_back({-inner_wall_x, -gap_size/2.0f, -inner_wall_z - inner_wall_thickness/2.0f, -inner_wall_z + inner_wall_thickness/2.0f});
    walls.push_back({gap_size/2.0f, inner_wall_x, -inner_wall_z - inner_wall_thickness/2.0f, -inner_wall_z + inner_wall_thickness/2.0f});

    walls.push_back({inner_wall_x - 2.0f, inner_wall_x, -inner_wall_z + inner_wall_thickness/2.0f, inner_wall_z - inner_wall_thickness/2.0f});
    walls.push_back({-inner_wall_x, -inner_wall_x + 2.0f, -inner_wall_z + inner_wall_thickness/2.0f, inner_wall_z - inner_wall_thickness/2.0f});
}

bool Scene::checkCollision(const Vec3& pos, float radius) {
    for (const auto& wall : walls) {
        if (pos.x + radius > wall.x_min && pos.x - radius < wall.x_max &&
            pos.z + radius > wall.z_min && pos.z - radius < wall.z_max) {
            return true;
        }
    }
    return false;
}

Vec3 Scene::resolveCollision(const Vec3& currentPos, const Vec3& desiredVel, double deltaTime, float radius) {
    Vec3 finalPos = currentPos;
    Vec3 adjustedVel = desiredVel;

    Vec3 testPosX = currentPos + Vec3(adjustedVel.x, 0, 0) * deltaTime;
    if (checkCollision(testPosX, radius)) {
        adjustedVel.x = 0;
    }

    Vec3 testPosZ = currentPos + Vec3(0, 0, adjustedVel.z) * deltaTime;
    if (checkCollision(testPosZ, radius)) {
        adjustedVel.z = 0;
    }
    
    finalPos = currentPos + adjustedVel * deltaTime;
    finalPos.y = currentPos.y;
    return finalPos;
}

void Scene::update(double deltaTime, GLFWwindow* window) {
    totalTime += deltaTime;
    const float time_powered_up_ends = 40.0f;
    const float time_animation_ends = 60.0f;
    const float exit_target_z = 7.0f;

    if (totalTime >= time_animation_ends && currentState != FINISHED) {
        currentState = FINISHED;
        glfwSetWindowShouldClose(window, true);
        return;
    }
    if (currentState == FINISHED) {
          pacman.velocity = Vec3(0,0,0);
          pacman.desiredVelocity = Vec3(0,0,0);
          for (auto& g : ghosts) {
              g.velocity = Vec3(0,0,0);
              g.desiredVelocity = Vec3(0,0,0);
          }
          return;
    }

    int ghosts_eaten_count = 0;
    for(const auto& g : ghosts) if (g.eaten) ghosts_eaten_count++;

    if (currentState == CHASING_PELLET && powerPellet.eaten) {
          currentState = EXITING_CENTER;
          pacman.poweredUp = true;
          pacman.velocity = Vec3(0,0,0);
          pacman.desiredVelocity = Vec3(0,0,0);
    } else if (currentState == EXITING_CENTER && pacman.position.z > exit_target_z - 0.5f) {
        currentState = POWERED_UP_CHASING;
    } else if (currentState == POWERED_UP_CHASING && (totalTime > time_powered_up_ends || ghosts_eaten_count >= 1 ) ) {
        currentState = VULNERABLE_FLEEING;
        pacman.poweredUp = false;
        targetGhost = nullptr;
    } else if (currentState == VULNERABLE_FLEEING && totalTime <= time_powered_up_ends && ghosts_eaten_count < 1) {
          currentState = POWERED_UP_CHASING;
          pacman.poweredUp = true;
    }

    Vec3 pacmanTargetDir;
    float pacmanSpeed;

    switch (currentState) {
        case CHASING_PELLET: {
             if (!powerPellet.eaten) {
                 float target_gap_x = 0.0f;
                 float intermediate_z_high = 7.0f;
                 float intermediate_z_low = -7.0f;
                 float path_z_target = (pacman.startPosition.z >= 0) ? intermediate_z_high : intermediate_z_low;

                 if (std::abs(pacman.position.z - path_z_target) > 0.5f && pacman.position.x < target_gap_x - 1.0f) {
                     pacmanTargetDir = Vec3(0, 0, (path_z_target > pacman.position.z) ? 1.0f : -1.0f);
                 } else if (pacman.position.x < target_gap_x - 0.5f) {
                     pacmanTargetDir = Vec3(1, 0, 0);
                 } else {
                     pacmanTargetDir = powerPellet.position - pacman.position;
                     pacmanTargetDir.x = 0;
                 }
                 if((powerPellet.position - pacman.position).lengthSq() < 1.0f * 1.0f) {
                     powerPellet.eaten = true;
                 }
             } else {
                   pacmanTargetDir = Vec3(1,0,0);
             }
             pacmanSpeed = pacman.speed;
             break;
        }
          case EXITING_CENTER: {
              pacmanTargetDir = Vec3(0, 0, 1);
              pacmanSpeed = pacman.speed;
              break;
          }
        case POWERED_UP_CHASING: {
              targetGhost = nullptr;
              float minDistSq = 10000.0f * 10000.0f;
              for(auto& g : ghosts) {
                  if (!g.eaten) {
                      float distSq = (g.position - pacman.position).lengthSq();
                      if (distSq < minDistSq) {
                          minDistSq = distSq;
                          targetGhost = &g;
                      }
                  }
              }
             if (targetGhost) {
                  pacmanTargetDir = targetGhost->position - pacman.position;
                  if (pacmanTargetDir.lengthSq() < 1.5f * 1.5f) {
                      targetGhost->eaten = true;
                  }
             } else {
                   pacmanTargetDir = (pacman.velocity.lengthSq() > 0.1f) ? pacman.velocity : getRandomDirection();
             }
             pacmanSpeed = pacman.speed;
             break;
        }
        case VULNERABLE_FLEEING: {
               Ghost* closestGhost = nullptr;
               float minDistSq = 10000.0f * 10000.0f;
                for(auto& g : ghosts) {
                      if (!g.eaten) {
                            float distSq = (g.position - pacman.position).lengthSq();
                            if (distSq < minDistSq) {
                                minDistSq = distSq;
                                closestGhost = &g;
                            }
                      }
                }
               if(closestGhost) {
                      pacmanTargetDir = pacman.position - closestGhost->position;
                } else {
                      pacmanTargetDir = pacman.velocity.lengthSq() > 0.1f ? pacman.velocity : getRandomDirection();
                }
             pacmanSpeed = pacman.flee_speed_boost;
             break;
        }
        case FINISHED:
              pacmanTargetDir = Vec3(0,0,0);
              pacmanSpeed = 0;
              break;
    }

    Vec3 pacmanVelForState = getAxisAligned(pacmanTargetDir) * pacmanSpeed;
    Vec3 currentPacmanDir = (pacman.velocity.lengthSq() > 0.1f) ? pacman.velocity.normalized() : pacmanVelForState.normalized();
    float lookAheadDist = pacman.radius + 0.2f; 
    
    if (currentPacmanDir.lengthSq() < 0.01f) currentPacmanDir = getRandomDirection();

    Vec3 lookAheadPos = pacman.position + currentPacmanDir * lookAheadDist;
    bool isBlockedByWall = checkCollision(lookAheadPos, pacman.radius);
    
    bool isBlockedByGhost = false;
    float interactDistSq = (pacman.radius + 1.0f) * (pacman.radius + 1.0f); 
    for (auto& otherGhost : ghosts) {
        if (!otherGhost.eaten && &otherGhost != targetGhost && (lookAheadPos - otherGhost.position).lengthSq() < interactDistSq) {
            isBlockedByGhost = true;
            break;
        }
    }

    if (isBlockedByWall || isBlockedByGhost) {
        Vec3 turnLeftVel = getTurnedLeft(currentPacmanDir) * pacmanSpeed;
        Vec3 turnRightVel = getTurnedRight(currentPacmanDir) * pacmanSpeed;

        Vec3 leftTestPos = pacman.position + turnLeftVel.normalized() * lookAheadDist;
        Vec3 rightTestPos = pacman.position + turnRightVel.normalized() * lookAheadDist;

        bool leftWall = checkCollision(leftTestPos, pacman.radius);
        bool leftGhost = false;
        for (auto& otherGhost : ghosts) {
            if (!otherGhost.eaten && &otherGhost != targetGhost && (leftTestPos - otherGhost.position).lengthSq() < interactDistSq) {
                leftGhost = true;
                break;
            }
        }
        bool canGoLeft = !leftWall && !leftGhost;

        bool rightWall = checkCollision(rightTestPos, pacman.radius);
        bool rightGhost = false;
        for (auto& otherGhost : ghosts) {
            if (!otherGhost.eaten && &otherGhost != targetGhost && (rightTestPos - otherGhost.position).lengthSq() < interactDistSq) {
                rightGhost = true;
                break;
            }
        }
        bool canGoRight = !rightWall && !rightGhost;

        if (canGoLeft && canGoRight) {
            pacman.desiredVelocity = (rand() % 2 == 0) ? turnLeftVel : turnRightVel;
        } else if (canGoLeft) {
            pacman.desiredVelocity = turnLeftVel;
        } else if (canGoRight) {
            pacman.desiredVelocity = turnRightVel;
        } else {
            pacman.desiredVelocity = currentPacmanDir * -1.0f * pacmanSpeed;
        }
    } else {
        Vec3 targetLookAheadPos = pacman.position + pacmanVelForState.normalized() * lookAheadDist;
        if (!checkCollision(targetLookAheadPos, pacman.radius)) {
            pacman.desiredVelocity = pacmanVelForState;
        } else {
            pacman.desiredVelocity = currentPacmanDir * pacmanSpeed;
        }
    }

    Vec3 currentPacmanPos = pacman.position;
    Vec3 resolvedPacmanPos = resolveCollision(pacman.position, pacman.desiredVelocity, deltaTime, pacman.radius);
    pacman.position = resolvedPacmanPos;

    if (deltaTime > 0.0001f) pacman.velocity = (resolvedPacmanPos - currentPacmanPos) / deltaTime;
    else pacman.velocity = Vec3(0,0,0);
    pacman.update(deltaTime);


    for (auto& g : ghosts) {
        if (!g.eaten) {
            Vec3 ghostTargetDir;
            float ghostSpeed;

            switch (currentState) {
                case CHASING_PELLET:
                    ghostTargetDir = pacman.position - g.position;
                    ghostSpeed = g.speed * 0.5f;
                    break;
                case EXITING_CENTER:
                case POWERED_UP_CHASING:
                    ghostTargetDir = g.position - pacman.position;
                    ghostSpeed = g.flee_speed;
                    break;
                case VULNERABLE_FLEEING:
                    ghostTargetDir = pacman.position - g.position;
                    ghostSpeed = g.speed * 0.6f;
                    break;
                case FINISHED:
                    ghostTargetDir = Vec3(0,0,0);
                    ghostSpeed = 0;
                    break;
            }

            Vec3 ghostVelForState = getAxisAligned(ghostTargetDir) * ghostSpeed;
            Vec3 currentGhostDir = (g.velocity.lengthSq() > 0.1f) ? g.velocity.normalized() : ghostVelForState.normalized();
            float ghostLookAheadDist = g.radius + 0.2f;

            if (currentGhostDir.lengthSq() < 0.01f) currentGhostDir = getRandomDirection();

            Vec3 ghostLookAheadPos = g.position + currentGhostDir * ghostLookAheadDist;
            bool isGhostBlockedByWall = checkCollision(ghostLookAheadPos, g.radius);
            
            bool isGhostBlockedByChar = false;
            float ghostInteractDistSq = (g.radius + 1.0f) * (g.radius + 1.0f);
            float pacmanInteractDistSq = (g.radius + pacman.radius) * (g.radius + pacman.radius);

            if ((currentState == POWERED_UP_CHASING || currentState == EXITING_CENTER) && (ghostLookAheadPos - pacman.position).lengthSq() < pacmanInteractDistSq) {
                isGhostBlockedByChar = true;
            } else if (currentState != POWERED_UP_CHASING && currentState != EXITING_CENTER && (ghostLookAheadPos - pacman.position).lengthSq() < pacmanInteractDistSq) {
                isGhostBlockedByChar = true;
            }

            if (!isGhostBlockedByChar) {
                for (auto& otherGhost : ghosts) {
                    if (!otherGhost.eaten && &g != &otherGhost && (ghostLookAheadPos - otherGhost.position).lengthSq() < ghostInteractDistSq) {
                        isGhostBlockedByChar = true;
                        break;
                    }
                }
            }

            if (isGhostBlockedByWall || isGhostBlockedByChar) {
                Vec3 turnLeftVel = getTurnedLeft(currentGhostDir) * ghostSpeed;
                Vec3 turnRightVel = getTurnedRight(currentGhostDir) * ghostSpeed;

                Vec3 leftTestPos = g.position + turnLeftVel.normalized() * ghostLookAheadDist;
                Vec3 rightTestPos = g.position + turnRightVel.normalized() * ghostLookAheadDist;

                bool leftWall = checkCollision(leftTestPos, g.radius);
                bool leftChar = false;
                if ((currentState == POWERED_UP_CHASING || currentState == EXITING_CENTER) && (leftTestPos - pacman.position).lengthSq() < pacmanInteractDistSq) {
                    leftChar = true;
                } else if (currentState != POWERED_UP_CHASING && currentState != EXITING_CENTER && (leftTestPos - pacman.position).lengthSq() < pacmanInteractDistSq) {
                      leftChar = true;
                }
                
                if (!leftChar) {
                    for (auto& otherGhost : ghosts) {
                        if (!otherGhost.eaten && &g != &otherGhost && (leftTestPos - otherGhost.position).lengthSq() < ghostInteractDistSq) {
                            leftChar = true;
                            break;
                        }
                    }
                }
                bool canGoLeft = !leftWall && !leftChar;

                bool rightWall = checkCollision(rightTestPos, g.radius);
                bool rightChar = false;
                if ((currentState == POWERED_UP_CHASING || currentState == EXITING_CENTER) && (rightTestPos - pacman.position).lengthSq() < pacmanInteractDistSq) {
                    rightChar = true;
                } else if (currentState != POWERED_UP_CHASING && currentState != EXITING_CENTER && (rightTestPos - pacman.position).lengthSq() < pacmanInteractDistSq) {
                    rightChar = true;
                }

                if (!rightChar) {
                    for (auto& otherGhost : ghosts) {
                        if (!otherGhost.eaten && &g != &otherGhost && (rightTestPos - otherGhost.position).lengthSq() < ghostInteractDistSq) {
                            rightChar = true;
                            break;
                        }
                    }
                }
                bool canGoRight = !rightWall && !rightChar;


                if (canGoLeft && canGoRight) {
                    g.desiredVelocity = (rand() % 2 == 0) ? turnLeftVel : turnRightVel;
                } else if (canGoLeft) {
                    g.desiredVelocity = turnLeftVel;
                } else if (canGoRight) {
                    g.desiredVelocity = turnRightVel;
                } else {
                    g.desiredVelocity = currentGhostDir * -1.0f * ghostSpeed;
                }
            } else {
                Vec3 targetLookAheadPos = g.position + ghostVelForState.normalized() * ghostLookAheadDist;
                  if (!checkCollision(targetLookAheadPos, g.radius)) {
                      g.desiredVelocity = ghostVelForState;
                  } else {
                     g.desiredVelocity = currentGhostDir * ghostSpeed;
                  }
            }
            
            Vec3 currentGhostPos = g.position;
            Vec3 resolvedGhostPos = resolveCollision(g.position, g.desiredVelocity, deltaTime, g.radius);
            g.position = resolvedGhostPos;
            
            if (deltaTime > 0.0001f) g.velocity = (resolvedGhostPos - currentGhostPos) / deltaTime;
            else g.velocity = Vec3(0,0,0);
        }
        g.update(deltaTime); 
    }
}

void Scene::render() {
    glClearColor(0.05f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
    GLfloat light_ambient[] = { 0.1f, 0.1f, 0.15f, 1.0f };
    GLfloat light_diffuse[] = { 0.7f, 0.7f, 0.9f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    GLfloat light_position1[] = { 10.0f, 2.0f, 10.0f, 1.0f };
    GLfloat light_diffuse1[] = { 0.8f, 0.4f, 0.1f, 1.0f };
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);

    GLfloat light_position2[] = { -10.0f, 2.0f, -10.0f, 1.0f };
    GLfloat light_diffuse2[] = { 0.1f, 0.4f, 0.8f, 1.0f };
    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);

    drawGround();
    drawSimpleMaze(walls);

    pacman.draw();

    for (auto& g : ghosts) {
        g.draw(pacman.poweredUp);
    }

    powerPellet.draw();

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_COLOR_MATERIAL);
}