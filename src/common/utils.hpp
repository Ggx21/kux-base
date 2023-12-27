#pragma once
#include<string>
#include<vector>

u_int32_t type_string_to_int(std::string type_string){
    if(type_string=="INT"){
        return 0;
    }
    else if(type_string=="FLOAT"){
        return 1;
    }
    else if(type_string=="VARCHAR"){
        return 2;
    }
    else{
        return -1;
    }
}

