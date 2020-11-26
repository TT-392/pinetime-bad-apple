int months[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; //an array with the amount of days per month

//leapyear is a function that returns one if the year you put in is a leap year
_Bool leapyear(int y){
    return (((y%4 == 0) && (y % 100 != 0)) || ((y%4 == 0) && (y % 400 == 0)));  //return if year is devidable by 4 but not by 100 or devidable by 4 and devidable by 400 (this means that it is a leap year)
}

//timetoepoch is a function that converts time to time since epoch
unsigned long timetoepoch(int year, int month, int day, int hour, int minute, int second) {
    unsigned long epoch = 0;              
    for(int i = 1970;i < year;i++){       //for every year since 1970
        epoch += (365+leapyear(i))*86400;   //add the amount of seconds in that year to epoch
    }
    for (int i=0;i < month-1;i++){        //for every month in the year
        epoch += months[i]*86400 ;      //add the amount of seconds in that month to epoch
    }
    epoch += (month > 1)*leapyear(year)*86400;  //add 1 day to epoch if it is a leap year and February is over
    epoch += (day-1)*86400;                     //add the amount of days-1(because the day isnt over yet) to epoch
    epoch += hour*3600;
    epoch += minute*60;
    epoch += second;   
    return(epoch);
}

//epochtotime is a function that converts time since epoch to normal time
void epochtotime(unsigned long timeSinceEpoch, int* second, int* minute, int* hour, int* day, int* month, int* year) {
    unsigned long epoch = timeSinceEpoch;
    int i = 1970;                             //the year when epoch started
    while(epoch/((365+leapyear(i))*86400)){   //while you can substract the amount of seconds in year i from epoch
        epoch -= (365+leapyear(i))*86400;       
        i++;                                    //add 1 to i
    }
    *year = i;                                 //after this i == year
    i = 0;                                    //make i 0 so you can do the same to months
    while(epoch/((months[i]*86400)+((i == 1)*leapyear(*year)*86400))){   //while you can substract the amount of seconds in month i from epoch
        epoch -= ((months[i]*86400)+((i == 1)*leapyear(*year)*86400));
        i++;                                                               //add 1 to i
    }
    *month = i+1;                      //the month is i+1(because we dont start counting from 0)
    *day = epoch/86400+1;
    *hour = (epoch%86400)/3600;
    *minute = (epoch%3600)/60;
    *second = epoch%60;                
}

