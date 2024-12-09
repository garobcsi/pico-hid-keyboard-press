/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+
void hid_task(void);

/*------------- MAIN -------------*/
int main(void)
{
    board_init();
    tusb_init();

    while (1)
    {
        tud_task(); // tinyusb device task
        hid_task(); // keyboard implementation
    }

    return 0;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report(bool keys_pressed)
{
    // skip if hid is not ready yet
    if (!tud_hid_ready())
    {
        return;
    }

    // avoid sending multiple zero reports
    static bool send_empty = false;

    if (keys_pressed)
    {
        uint8_t key_codes[6] = {0};
        key_codes[0] = 0x0A;
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, key_codes);
        send_empty = true;
    }
    else
    {
        // send empty key report if previously has key pressed
        if (send_empty)
        {
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        }
        send_empty = false;
    }
}


// Every 10ms, we poll the pins and send a report
void hid_task(void)
{
    board_led_write(false);

    send_hid_report(false);

    // Poll every 10ms
    const uint32_t interval = 90000;
    static uint32_t last_keypress_time = 0;
    uint32_t current_time = board_millis();
    
    if (current_time - last_keypress_time < interval)
    {
        return; // not enough time 
    }
    last_keypress_time = current_time;

    // send a report
    send_hid_report(true);
    send_hid_report(false);

    board_led_write(true);
    sleep_ms(50);
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint8_t len)
{
    // not implemented, we only send REPORT_ID_KEYBOARD
    (void)instance;
    (void)len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance;
}