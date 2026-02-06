#include <sil/sil.hpp>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "random.hpp"
#include <array>

void keep_green_only(sil::Image& image){
    for (glm::vec3& color : image.pixels()){
        color.r = 0.f;
        color.b = 0.f;
    }
}

glm::vec3 setGrey(glm::vec3 color, int grey){
    color.r = grey;
    color.g = grey;
    color.b = grey;
}

void channels_swap(sil::Image& image){
    for (glm::vec3& color : image.pixels()){
        std::swap(color.r,color.b);
    }
}

void NoirEtBlanc(sil::Image& image){
    for (glm::vec3& color : image.pixels()){
        float moy = (color.r*0.30f+color.b*0.53f+color.g*0.99f )/3.f;
        color = setGrey(color,moy);
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

void disk(sil::Image& image,int Ox,int Oy ,int r){
    for (int x{0}; x < image.width(); x++){
        for (int y{0}; y < image.height(); y++){
            if((x-Ox)*(x-Ox)+(y-Oy)*(y-Oy)<=r*r){
                image.pixel(x, y) = {0,1,0};
            }
        }
    }
}

void circle(sil::Image& image,int Ox,int Oy ,int r){
    for (int x{0}; x < image.width(); x++){
        for (int y{0}; y < image.height(); y++){
            int position = (x-Ox)*(x-Ox)+(y-Oy)*(y-Oy);
            if(position<=r*r&&position>=(r-5)*(r-5)){
                image.pixel(x, y) = {0,1,0};
            }
        }
    }
}

void rosace(sil::Image& image,int Ox,int Oy,int size){
    circle(image,Ox,Oy,size);
    for(int i{0}; i <361;i+=60){
        circle(image, int(size*cos(i*3.14159/180))+Ox, int(size*sin(i*3.14159/180))+Oy,size);
    }
}
void darken(sil::Image& image){
    for (glm::vec3& color : image.pixels()){
        color *= color;
    }
}

void lighten(sil::Image& image){
    for (glm::vec3& color : image.pixels()){
        color = sqrt(color);
    }
}

void mosaic(sil::Image& image){
    sil::Image imageBig{image.width()*5, image.height()*5};
    bool alternateX{true};
    bool alternateY{true};
    for(int j{0}; j <imageBig.height();j+=image.height()){
        for(int i{0}; i <imageBig.width();i+=image.width()){
            for (int x{0}; x < image.width(); x++){
                for (int y{0}; y < image.height(); y++){
                    imageBig.pixel(x+i, y+j) = image.pixel(alternateX ? x : image.width()-1-x, alternateY ? y : image.height()-1-y);
                }
            }
            alternateX = !alternateX;
        } alternateY = !alternateY;
        alternateX = !alternateX;
    }
    image = imageBig;
}

void glitch(sil::Image& image){
    for (int posX{0}; posX < image.width()-40; posX++){
        for (int posY{0}; posY < image.height()-10; posY++){
            if(true_with_probability(0.0005f)){
                int i = random_int(10, 50);
                int j = random_int(1, 10);
                int  coordA = random_int(1, image.width()-50);
                int coordB = random_int(1, image.height()-10);
                for(int a{0};a<i;a++){
                    for(int b{0};b<j;b++){
                        std::swap(image.pixel(posX+a,posY+b),image.pixel(coordA+a,coordB+b));
                    }
                }
            }
        }
    }
}

float brightness(glm::vec3 color){
    return (color.r*0.30f+color.b*0.53f+color.g*0.99f)/3.f;
}

void sort(sil::Image& image){
    for (int posX{0}; posX < image.width(); posX++){
        for (int posY{0}; posY < image.height()-50; posY++){
            if(true_with_probability(0.05f)){
                int  j = random_int(1, 50);
                sil::Image tab{1,j};
                for(int i{0};i<j;i++){    
                    tab.pixel(0,i)=image.pixel(posX,posY+i);
                }
                std::sort(tab.pixels().begin(),tab.pixels().end(),[](glm::vec3 const& color1, glm::vec3 const& color2){
                return brightness(color1) < brightness(color2);});
                for(int i{0};i<j;i++){    
                    image.pixel(posX,posY+i)=tab.pixel(0,i);
                }
            }
        }
    }
}



void dithering(sil::Image& image){
    std::array<std::array<float, 2>, 2> bayer{{ {0.f, 2.f}, {3.f, 1.f} }};
    for (int x{0}; x < image.width(); x++){
        for (int y{0}; y < image.height(); y++){
            glm::vec3& color = image.pixel(x, y);
            color = setGrey(color, (brightness(color)<((bayer[x%2][y%2]+ 0.5f)/6.f))<0.5? 1.f : 0.f);
        }
    }
}


void normalise(sil::Image& image){
    float min{1.f};
    float max{0.f};

    for (glm::vec3& color : image.pixels()){
        float b = brightness(color);
        if (b < min) min = b;
        if (b > max) max = b;
    }

    float range = max - min;
    if (range < 1e-6f) return;

    for (glm::vec3& color : image.pixels()){
        float b = brightness(color);
        float bNorm = (b - min) / range;
        color *= (bNorm / b);
    }
}

void convolution(sil::Image& image){
    sil::Image imageBlur {image};
    for (int x{0}; x < image.width(); x++){
        for (int y{0}; y < image.height(); y++){
            glm::vec3 color = image.pixel(x, y);
            float moy = 1.f;
            for(int i{1};i<15;i++){
                if(x+i<image.width()){
                    color+=image.pixel(x+i,y);
                    moy++;
                }
                if(x-i>0.f){
                    color+=image.pixel(x-i,y);
                    moy++;
                }
                if(y+i<image.height()){
                    color+=image.pixel(x,y+i);
                    moy++;
                }
                if(y-i>0.f){
                    color+=image.pixel(x,y-i);
                    moy++;
                }
            }
            imageBlur.pixel(x, y) = color / moy;
        }
    }
    image = imageBlur;
}

void saveAll(){
    {
        sil::Image image{"images/logo.png"};
        keep_green_only(image);
        image.save("output/keep_green_only.png");
    }
    {
        sil::Image image{"images/logo.png"};
        channels_swap(image);
        image.save("output/channels_swap.png");
    }
    {
        sil::Image image{"images/photo.jpg"};
        NoirEtBlanc(image);
        image.save("output/NoirEtBlanc.png");
    }
    {
        sil::Image image{"images/logo.png"};
        negative(image);
        image.save("output/negative.png");
    }
    {
        sil::Image image{300/*width*/, 200/*height*/};
        degrade(image);
        image.save("output/degrade.png");
    }
    {
        sil::Image image{"images/logo.png"};
        mirroir(image);
        image.save("output/mirroir.png");
    }
    {
        sil::Image image{"images/logo.png"};
        rotate(image);
        image.save("output/rotate.png");
    }
    {
        sil::Image image{300/*width*/, 300/*height*/};
        disk(image, image.width()/2,image.height()/2,100);
        image.save("output/disk.png");
    }
    {
        sil::Image image{300/*width*/, 300/*height*/};
        circle(image, image.width()/2,image.height()/2,100);
        image.save("output/circle.png");
    }
    {
        sil::Image image{"images/photo.jpg"};
        lighten(image);
        image.save("output/lighten.png");
    }
    {
        sil::Image image{"images/photo.jpg"};
        darken(image);
        image.save("output/darken.png");
    }
    {
        sil::Image image{300/*width*/, 300/*height*/};
        rosace(image,image.width()/2,image.height()/2,70);
        image.save("output/rosace.png");
    }
    {
        sil::Image image{"images/logo.png"};
        mosaic(image);
        image.save("output/mosaic.png");
    }
    {
        sil::Image image{"images/logo.png"};
        sort(image);
        image.save("output/sort.png");
    }
    {
        sil::Image image{"images/photo.jpg"};
        dithering(image);
        image.save("output/dithering.png");
    }
    {
        sil::Image image{"images/photo.jpg"};
        normalise(image);
        image.save("output/normalise.png");
    }
    {
        sil::Image image{"images/logo.png"};
        convolution(image);
        image.save("output/convolution.png");
    }
}

int main(){
    saveAll();
    //sil::Image image{"images/logo.png"};
    //sil::Image image{300/*width*/, 200/*height*/};
    //keep_green_only(image);
    //channels_swap(image);
    //NoirEtBlanc(image);
    //negative(image);
    //degrade(image);
    //mirroir(image);
    //rotate(image);
    //disk(image, image.width()/2,image.height()/2,100);
    //circle(image, image.width()/2,image.height()/2,100);
    //lighten(image);
    //darken(image);
    //rosace(image,image.width()/2,image.height()/2,120);
    //mosaic(image);
    //sort(image);
    //dithering(image);
    //normalise(image);
    //convolution(image);
    //image.save("output/pouet.png");
}