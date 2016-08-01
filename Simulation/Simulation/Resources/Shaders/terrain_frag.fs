#version 330


in vec4 colour;
in vec4 posFrag;
in float changingNumber;
in float randomNumberCol;
flat in int fragMode;//remove interpolation. 

layout (location=0) out vec4 fragColour;

void main(void) {



    if (fragMode == 1)
    {
          if(fract(posFrag.x) > 0.95 || fract(posFrag.z) > 0.95 || fract(posFrag.x) < 0.05 || fract(posFrag.z) < 0.05)
    {
        fragColour = vec4(0,0,0,1);
    }
    else
    {
        if (posFrag.x > 7 && posFrag.z > 7)
        {
            fragColour = vec4(0.4,0.4,0.4,1);
        }
        else
        {
            fragColour = vec4(0,0.3,0,1);
        }
        
    }
    
    }
    if (fragMode == 2)
    {
    
    if(fract(posFrag.x) > 0.95 || fract(posFrag.z) > 0.95 || fract(posFrag.x) < 0.05 || fract(posFrag.z) < 0.05)
    {
        fragColour = vec4(0,0,0,1);
    }
    else
    {   
        if(posFrag.y > 0.5)
        {
            fragColour = vec4(0,0.5,0.9,1);
        }
        else
        {
            fragColour = vec4(0,0.2,0.6,1);
        }
        if(posFrag.y < 0.2)
        {
            fragColour = vec4(0,0.1,0.2,1);
        }
    }
    
    }
    if (fragMode == 3)
    {
    
      if(fract(posFrag.x) > 0.95 || fract(posFrag.z) > 0.95 || fract(posFrag.x) < 0.05 || fract(posFrag.z) < 0.05)
    {
        fragColour = vec4(0,0,0,1);
    }
    
        if(posFrag.y > 0.25)
        {
            fragColour = vec4(0.25,0,0,1);
        }
        if(posFrag.y > 0.5)
        {
            fragColour = vec4(0.5,0,0,1);
        }
        if(posFrag.y > 0.75)
        {
            fragColour = vec4(0.75,0,0,1);
        }
        if(posFrag.y >1)
        {
            fragColour = vec4(1,0,0,1);
        }
        if (posFrag.y < 0.25)
        {
            fragColour = vec4(0.2,0,0,1);
        }
        if (posFrag.y < 0.15)
        {
            fragColour = vec4(0.15,0,0,1);
        }
        if (posFrag.y < 0.1)
        {
            fragColour = vec4(0.1,0,0,1);
        }
    }
   
    if (fragMode == 4)
    {
    
     if(fract(posFrag.x) > 0.95 || fract(posFrag.z) > 0.95 || fract(posFrag.x) < 0.05 || fract(posFrag.z) < 0.05)
    {
        fragColour = vec4(0,0,0,1);
    }

    
    else
    {
       fragColour = vec4(sin(posFrag.x/10 + changingNumber),(posFrag.z/10)*randomNumberCol * posFrag.y,posFrag.y * (posFrag.y/10)* randomNumberCol,1);
    }
    
    }
    if (fragMode == 5)
    {
    
      if(fract(posFrag.x) > 0.98 || fract(posFrag.z) > 0.98 || fract(posFrag.x) < 0.02 || fract(posFrag.z) < 0.02)
        {
            fragColour = vec4(0,0,0,1);
        }
        else
        {
            if(posFrag.y < -0.05)
            {
                fragColour = vec4(0,0.1,0.4,1);
            }
            else
            {
                fragColour = vec4(0,0.2,0.5,1);
            }
        }
    
    }
    if (fragMode == 6)
    {
      
      if(fract(posFrag.x) > 0.95 || fract(posFrag.z) > 0.95 || fract(posFrag.x) < 0.05 || fract(posFrag.z) < 0.05)
        {
             fragColour = vec4(0,0,0,1);
        }
        else
        {
            if(fract(cos(posFrag.x+changingNumber)) < 1 || fract(posFrag.z+changingNumber) < 1 || fract(posFrag.x+changingNumber) > 0 || fract(posFrag.z+changingNumber) > 0)
            {
               fragColour = vec4(cos(changingNumber),0,sin(changingNumber),1);
            }
            if(fract(posFrag.x) < 0.85 && fract(posFrag.z) < 0.85 && fract(posFrag.x) > 0.15 && fract(posFrag.z) > 0.15)
            {
               fragColour = vec4(0.5,0.5,0,1);
            }
            if(fract(posFrag.x) < 0.75 && fract(posFrag.z) < 0.75 && fract(posFrag.x) > 0.25 && fract(posFrag.z) > 0.25)
            {
             fragColour = vec4(0,1,0,1);
            }
            if(fract(posFrag.x) < 0.65 &&  fract(posFrag.z) < 0.65 &&  fract(posFrag.x) > 0.35 &&  fract(posFrag.z) > 0.35)
            {
               fragColour = vec4(0,0.5,0.5,1);
            }
            if(fract(posFrag.x) < 0.6 && fract(posFrag.z) < 0.6 && fract(posFrag.x) > 0.4 && fract(posFrag.z) > 0.4)
            {
             fragColour = vec4(0,0,1,1);
            }
             
        }
    
    }

}
