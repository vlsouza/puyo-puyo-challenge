#include "types.h"

class Block {
public:
    Block(const u8* data, s32 side){
        data = data;
        side = side;
    }; 
private:
    const u8* data;
    s32 side;
};
