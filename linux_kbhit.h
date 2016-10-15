//
//  linux_kbhit.h
//  project1
//
//  Created by Sungmin Im on 2016. 10. 11..
//  Copyright © 2016년 Sungmin Im. All rights reserved.
//

#ifndef linux_kbhit_h
#define linux_kbhit_h

#include <stdio.h>
#include <termios.h>
#include <unistd.h>



int linux_kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    

    
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    
    
    
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    
    
    
    ch = getchar();
    
    
    
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    
    
    
    return ch;
}

#endif /* linux_kbhit_h */
