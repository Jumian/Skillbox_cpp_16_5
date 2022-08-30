#include <iostream>
#include "cstdio"
#include "sstream"
#include "cmath"

enum switches
{
    LIGHTS_INSIDE = 1,
    LIGHTS_OUTSIDE = 2,
    HEATERS = 4,
    WATER_PIPE_HEATING = 8,
    CONDITIONER = 16
};
int lamp_temp = 5000;

int switches_state=0;
int hour=0;
double temperature_outside=0,temperature_inside=0;
bool light_inside_b=false, movement_b=false;

bool switch_state(int old_state, int new_state, int flag){
    return ((old_state & flag) != (new_state & flag));
}

void print_lamp_temp(int old_state,int new_state,int lamp_temp_old, int lamp_temp_new){
    if ((lamp_temp_old!=lamp_temp_new)&&(switches_state & LIGHTS_INSIDE))
        std::cout << "light temperature changed " << lamp_temp_old<<"K -> " << lamp_temp_new << "K" << std::endl;
    else if (new_state & LIGHTS_INSIDE) std::cout << "light temperature = " <<  lamp_temp_new << "K" << std::endl;
}

void print_switches(int old_state,int new_state,int lamp_temp_old, int lamp_temp_new){
    if (switch_state( old_state, new_state,LIGHTS_INSIDE))
        std::cout << "Lights inside switched "<<(new_state&LIGHTS_INSIDE?"on":"off") << std::endl;
    print_lamp_temp( old_state, new_state,lamp_temp_old, lamp_temp_new);
    if (switch_state( old_state, new_state,LIGHTS_OUTSIDE))
        std::cout << "Lights outside switched "<<(new_state&LIGHTS_OUTSIDE?"on":"off") << std::endl;

    if (switch_state( old_state, new_state,HEATERS))
        std::cout << "Heaters switched "<<(new_state&HEATERS?"on":"off") << std::endl;

    if (switch_state( old_state, new_state,WATER_PIPE_HEATING))
        std::cout << "Water pipe heating switched "<<(new_state&WATER_PIPE_HEATING?"on":"off") << std::endl;

    if (switch_state( old_state, new_state,CONDITIONER))
        std::cout << "Conditioner switched "<<(new_state&CONDITIONER?"on":"off") << std::endl;
}

int check_conditions(){
    int switches_state_new=switches_state;
    if (temperature_outside < 0) switches_state_new |= WATER_PIPE_HEATING;
    if (temperature_outside > 5) switches_state_new &= ~WATER_PIPE_HEATING;
    if ((hour>16 || hour < 5) && movement_b) switches_state_new |= LIGHTS_OUTSIDE;
    else switches_state_new &= ~LIGHTS_OUTSIDE;
    if (temperature_inside < 22) switches_state_new |= HEATERS;
    if (temperature_inside <= 25) switches_state_new &= ~CONDITIONER;
    if (temperature_inside > 30) switches_state_new |= CONDITIONER;
    if (temperature_inside >= 25) switches_state_new &= ~HEATERS;
    if (light_inside_b) switches_state_new |= LIGHTS_INSIDE;
    else switches_state_new &= ~LIGHTS_INSIDE;
    int lamp_temp_new=lamp_temp;
    if (hour>=16&&hour <= 20) lamp_temp_new=5000+(2700-5000)/(20-16)*(hour-16);
    else if (hour==0) lamp_temp_new=5000;
    print_switches(switches_state,switches_state_new,lamp_temp,lamp_temp_new);
    lamp_temp=lamp_temp_new;
    return switches_state_new;
}

bool stream_parse(std::stringstream &stream){
    std::string light_inside_s="", movement_s="";
    stream >> temperature_outside >> temperature_inside >> movement_s>> light_inside_s  ;
    if (movement_s == "yes") movement_b= true;
    else if (movement_s == "no") movement_b= false;
    else return false;
    if (light_inside_s == "on") light_inside_b= true;
    else if (light_inside_s == "off") light_inside_b= false;
    else return false;
    if (!std::isfinite(temperature_outside)) return false;
    if (!std::isfinite(temperature_inside)) return false;
    return true;
}

int main() {
    std::string input="";
    double epsylon=0.0001,quit_number=-273;
    std::cout<< "Welcome! Input inside temp " << quit_number<< " for exit."<<std::endl;
    char buf[100];
    bool notParsed=true;
    do{
        std::sprintf(buf,"Time is %i:00. Input: temperature outside, temperature inside, movement outside[yes/no],light inside[on/off]:",
                     hour);
        std::cout << buf<< std::endl;
        std::getline(std::cin, input);
        std::stringstream sstring(input);
        notParsed = !stream_parse(sstring);
        switches_state = check_conditions();
        ++hour;
        if (hour>23) hour = 0;
    }while(notParsed|| temperature_inside>epsylon+quit_number || temperature_inside<-epsylon+quit_number);
}
