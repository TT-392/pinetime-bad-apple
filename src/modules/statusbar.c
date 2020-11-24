void statusBar_refresh() {
    if (secondPassed) {
        uint16_t color = 0xffff; 
        if (!nrf_gpio_pin_read(19))
            color = 0x67EC;
        battery_percent(200,0,color,0x0000);
        drawDate(0,0,"%H:%M:%S");
        secondPassed = 0;
    } 
}
