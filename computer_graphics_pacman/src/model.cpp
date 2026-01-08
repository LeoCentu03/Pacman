#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "model.h"
#include <GL/gl.h>
#include <iostream>

bool Model::load(const std::string& filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
        std::cerr << "Errore caricamento modello: " << filename << " -> " << warn << err << std::endl;
        return false;
    }

    vertices.clear();
    normals.clear();

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            if (!attrib.normals.empty() && index.normal_index >= 0) {
                normals.push_back(attrib.normals[3 * index.normal_index + 0]);
                normals.push_back(attrib.normals[3 * index.normal_index + 1]);
                normals.push_back(attrib.normals[3 * index.normal_index + 2]);
            }
        }
    }
     if (normals.empty() && !vertices.empty()) {
        normals.resize(vertices.size());
        for(size_t i = 0; i < vertices.size() / 3; ++i) {
            normals[i*3 + 0] = 0.0f;
            normals[i*3 + 1] = 1.0f;
            normals[i*3 + 2] = 0.0f;
        }
    } else if (normals.size() != vertices.size()) {
         std::cerr << "Attenzione: Mismatch normali/vertici per " << filename << ". Normali ignorate.\n";
         normals.clear();
    }

    std::cout << "Modello caricato: " << filename << ", Vertici: " << vertices.size() / 3 << std::endl;
    return true;
}

void Model::draw() {
    if (vertices.empty()) return;

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices.data());

    if (!normals.empty()) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, normals.data());
    }

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 3));

    glDisableClientState(GL_VERTEX_ARRAY);
    if (!normals.empty()) {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
}