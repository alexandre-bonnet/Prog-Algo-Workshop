#include <sil/sil.hpp>
#include <iostream>

void keep_green_only(sil::Image& image){
    for (glm::vec3& color : image.pixels()){
        color.r = 0.f;
        color.b = 0.f;
    }
}

void channels_swap(sil::Image& image){
    for (glm::vec3& color : image.pixels()){
        std::swap(color.r,color.b);
    }
}

void NoirEtBlanc(sil::Image& image){
    for (glm::vec3& color : image.pixels()){
        float moy = (color.r*0.30f+color.b*0.53f+color.g*0.99f )/3.f;
        color.r = moy;
        color.g = moy;
        color.b = moy;
    }
}

void negative(sil::Image& image){
    for (glm::vec3& color : image.pixels()){
        color = 1.f-color;
    }
}

void degrade(sil::Image& image){
    glm::vec3 color = {0,0,0};
    for (int x{0}; x < image.width(); x++){
        for (int y{0}; y < image.height(); y++){
            image.pixel(x, y) = color;
        } color += 1.f/float(image.width());
    }
}

void mirroir(sil::Image& image){
    sil::Image image2 = image;
    for (int x{0}; x < image.width(); x++){
        for (int y{0}; y < image.height(); y++){
            image2.pixel(x, y) = image.pixel(image.width()-1-x, y);
        }
    }
    image = image2;
}

void rotate(sil::Image& image){
    sil::Image imageRot{image.height(), image.width()};
    for (int x{0}; x < image.width(); x++){
        for (int y{0}; y < image.height(); y++){
                imageRot.pixel(image.height()-1-y, x) = image.pixel(x, y);
        }
    } image = imageRot;
}

void cercle(sil::Image& image,int Ox,int Oy ,int r){
    for (int x{0}; x < image.width(); x++){
        for (int y{0}; y < image.height(); y++){
            if((x-Ox)*(x-Ox)+(y-Oy)*(y-Oy)<=r*r){
                image.pixel(x, y) = {0,1,0};
            }
        }
    }
}


int main(){
    sil::Image image{"images/logo.png"};
    //sil::Image image{300/*width*/, 200/*height*/};

    //keep_green_only(image);
    //channels_swap(image);
    //NoirEtBlanc(image);
    //negative(image);
    //degrade(image);
    //mirroir(image);
    cercle(image, image.width()/2,image.height()/2,100);
    //rotate(image);
    image.save("output/pouet.png");
}