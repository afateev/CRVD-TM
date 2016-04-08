
#ifndef _MEDIA_SD_RESPONSE_BUFFER_H_
#define _MEDIA_SD_RESPONSE_BUFFER_H_


namespace Rblib
{
namespace Media
{
namespace Sd
{
namespace Response
{
    
    //
    // Response buffer
    //

    class Buffer
    {
    public:

        Buffer(unsigned char* buffer, unsigned char length)
        : _buffer(buffer)
        , _length(length)
        {
        }

        inline unsigned char* Ptr()
        {
            return _buffer;
        }

        inline unsigned char Length()
        {
            return _length;
        }

    private:

        unsigned char* _buffer;
        unsigned char _length;

    };
}
}
}
}

#endif // _MEDIA_SD_RESPONSE_BUFFER_H_

