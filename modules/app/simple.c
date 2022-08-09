#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "data_struct.pb.h"

int main()
{
    uint8_t mysimplemessagebuf[MySimpleMessage_size];
    memset(mysimplemessagebuf, 0, MySimpleMessage_size);
    size_t message_length;
    // Encode our message 
    {
        MySimpleMessage message;
        message.my_number = 15;
        pb_ostream_t enc_stream = pb_ostream_from_buffer(mysimplemessagebuf, MySimpleMessage_size);
        if (!pb_encode(&enc_stream, MySimpleMessage_fields, &message)) {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&enc_stream));
        } else {
            message_length = enc_stream.bytes_written;
        }
    }
    // Decode our message 
    {
        MySimpleMessage message;
        pb_istream_t dec_stream = pb_istream_from_buffer(mysimplemessagebuf, message_length);

        if (!pb_decode(&dec_stream, MySimpleMessage_fields, &message)) {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&dec_stream));
        }
        printf("Your lucky number was %d!\n", (int)message.my_number);
    }
    
    return 0;
}