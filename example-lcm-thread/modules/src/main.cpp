#include <inttypes.h>
#include <lcm/lcm.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <pb_encode.h>
#include <pb_decode.h>
#include "data_struct.pb.h"


std::mutex message_mutex_;
static MySimpleMessage message_info_t;

bool message_info_updated = false;

MySimpleMessage* message_info_ptr;

MySimpleMessage message_info;
MySimpleMessage message_info_tmp;

void struct_update() {
    message_mutex_.lock();
    if (message_info_updated) {
        message_info = message_info_tmp;
        message_info_updated = false;
        message_info_ptr = &message_info;
    }
    message_mutex_.unlock();
}

bool message_info_update(MySimpleMessage* const message) {
    message_mutex_.lock();
    // message->my_number++;
    message_info_tmp = *message;
    message_info_updated = true;
    message_mutex_.unlock();
    return true;
}

void sub_handler(const lcm_recv_buf_t* rbuf, const char* channel, void* user_data)
{
    pb_istream_t dec_stream;
    int rxlen = 0;
    dec_stream = pb_istream_from_buffer((uint8_t*)rbuf->data, rbuf->data_size);
    if (!pb_decode(&dec_stream, MySimpleMessage_fields, &message_info_t)) {
        printf("chassis pb decode error in %s\n", __func__);
    } else {
        printf("Subscribe number was %d!\n", message_info_t.my_number);
        message_info_update(&message_info_t);
    }
}
void lcm_sub() {
    lcm_t *lcm_sub = lcm_create(NULL);
    char channel_name[] = "channel";
    if (!lcm_sub) printf("lcm_t creat failed\n");
    lcm_subscribe(lcm_sub, channel_name, &sub_handler, NULL);
    while (1) {
        lcm_handle(lcm_sub);
        usleep(10000);// 10ms
    }
}
void lcm_pub() {
    uint8_t mysimplemessagebuf[MySimpleMessage_size];
    memset(mysimplemessagebuf, 0, MySimpleMessage_size);
    size_t message_length;
    char channel_name[] = "channel";
    lcm_t *lcm_pub = lcm_create(NULL);
    if (!lcm_pub) printf("lcm_t creat failed\n");
    while(1) {
        struct_update();
        pb_ostream_t enc_stream = pb_ostream_from_buffer(mysimplemessagebuf, MySimpleMessage_size);
        if (!pb_encode(&enc_stream, MySimpleMessage_fields, &message_info_t)) {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&enc_stream));
        } else {
            message_length = enc_stream.bytes_written;
            // printf("Encoding succeed! \n");
            printf("Publish number was %d!\n", (int)message_info_t.my_number);
        }
        lcm_publish(lcm_pub, channel_name, (mysimplemessagebuf), message_length);
        usleep(50000);// 50ms
    }
}

void statemechine() {
    while(1) {
        message_info_t.has_my_number = true;
        message_info_t.my_number++;
        usleep(50000);// 50ms
    }
    
}

int main() {
    std::thread t_subscriber = std::thread(&lcm_sub);
    std::thread t_publisher = std::thread(&lcm_pub);
    std::thread t_statemechine = std::thread(&statemechine);
    while (true) {
        usleep(1000000);
    }
    t_subscriber.join();
    t_publisher.join();
    t_statemechine.join();

    return 0;
}