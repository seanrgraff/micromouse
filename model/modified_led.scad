// File: modified_led.scad
// Author: Aksel Thomas
// Date: 2/22/2017

include <model_constants.scad>

module modified_led(xt = 0, yt = 0, zt = 0, color = "red") {
    translate([xt, yt, zt]) {
        translate([0, 0, 0]) {
            color(color)
            cylinder(d = LED_CASE_D, h = LED_CASE_H, center = false);
        }
        translate([0, LED_LEAD_OFFSET, -LED_LEAD_SHORT_H]) {
            color("silver")
            cylinder(d = MODIFIED_LED_LEAD_D, h = LED_LEAD_SHORT_H, center = false);
        }
        translate([0, -LED_LEAD_OFFSET, -LED_LEAD_LONG_H]) {
            color("silver")
            cylinder(d = MODIFIED_LED_LEAD_D, h = LED_LEAD_LONG_H, center = false);
        }
    }
}
