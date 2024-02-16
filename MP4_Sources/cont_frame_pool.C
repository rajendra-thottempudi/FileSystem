/*
 File: ContFramePool.C
 
 Author: Rajendra Thottempudi	
 Date  : 14 September, 2022
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
#define MB * (0x1 << 20)
#define KB * (0x1 << 10)

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

ContFramePool* ContFramePool::framepool_head;
//head of the frame pool
ContFramePool* ContFramePool::framepool_list;
//list of frame pools

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

/* 

 I have declared the following notations to store the state of frames in two bits :
  00 - Indicates that the frame is FREE
  11 - Indicates that the frame is allocated
  01 - Indicates that the frame is the head of a contiguous set of frames
  10 - Indicates that the frame is inaccessible	
 */

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no)
{
    assert(_n_frames <= FRAME_SIZE*4);

    base_frame_no = _base_frame_no;
    nframes = _n_frames;
    nFreeFrames = _n_frames;
    info_frame_no = _info_frame_no;

    if(info_frame_no == 0) {
        bitmap = (unsigned char *) (base_frame_no * FRAME_SIZE); 
    } else {
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);
    }

    assert ((nframes % 8 ) == 0);
    
    int i = 0;
    for(i; i*8 < _n_frames*2; i++) {
        bitmap[i] = 0x0;
    }

    if(_info_frame_no == 0) {
        bitmap[0] = 0x40;
        nFreeFrames--;
    }

    if (ContFramePool::framepool_head == NULL) {
        ContFramePool::framepool_head = this;
        ContFramePool::framepool_list = this;
    } else {
        ContFramePool::framepool_list->framepool_next = this;
        ContFramePool::framepool_list = this;
    }
    framepool_next = NULL;
 
    Console::puts("The Frame Pool has been initialized successfully\n");
}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    unsigned int frames_required = _n_frames;
    unsigned int frame_no = base_frame_no;
    int startSearch = 0; // a O indicates that we need to start the search and dont have any head as of now
    int found = 0;
    int index_Of_Bitmap = 0;
    int index_Within_char = 0;
    int endSearch = 0;
    int bitnumber = 0;
    

    if(_n_frames > nFreeFrames) {
        Console::puts("The required number of contiguous frames are not available at this moment!");
    }

    for (unsigned int j = 0;j<nframes/4; j++) {
        unsigned char currentCharValue = bitmap[j];
        unsigned char maskedElement = 0xC0;
        for (int k = 0; k < 4; k++) {
            if((bitmap[j] & maskedElement) == 0) {
                if(startSearch == 1) {
                    frames_required--;
                    endSearch = 1;
                    bitnumber++;
                } else {
                    startSearch = 1;
                    frame_no += j*4 + k;
                    index_Of_Bitmap = j;
                    index_Within_char = k;
                    frames_required--;
                    bitnumber--;
                }
            } else {
                if(startSearch == 1) {
                    frame_no = base_frame_no;
                    frames_required = _n_frames;
                    index_Of_Bitmap = 0;
                    index_Within_char = 0;
                    startSearch = 0;
                    endSearch = 0;
                }
            }
            maskedElement = maskedElement>>2;
            if (frames_required == 0) {
                found = 1;
                break;
            }
        } 
        if (frames_required == 0) {
            found = 1;
            break;
        }
    } 

    if (found == 0 ) {
        Console::puts("Unable to find a free frame for the given length :");Console::puti(_n_frames);Console::puts("\n");
        return 0;
    }

    int required_frames = _n_frames;
    int finalNumberOfFrames = 0;
    unsigned char first_mask = 0x40;
    unsigned char second_mask = 0xC0;
    first_mask = first_mask>>(index_Within_char*2);
    second_mask = second_mask>>(index_Within_char*2);
    bitmap[index_Of_Bitmap] = (bitmap[index_Of_Bitmap] & ~second_mask)| first_mask; //setting the bits to 01
    
    index_Within_char++;
    required_frames--;

    unsigned char primary_mask = 0xC0;
    primary_mask = primary_mask>>(index_Within_char*2);
    while(required_frames > 0 && index_Within_char < 4) {
        bitmap[index_Of_Bitmap] = bitmap[index_Of_Bitmap] | primary_mask;
        primary_mask = primary_mask>>2;
        required_frames--;
        index_Within_char++;
    }
    

    for(int i = index_Of_Bitmap + 1; i< nframes/4; i++) {
        primary_mask = 0xC0;
        for (int j = 0; j< 4 ; j++) {
            if (required_frames == 0) {
                break;
            }
            bitmap[i] = bitmap[i] | primary_mask;
            primary_mask = primary_mask>>2;
            required_frames--;
        }
        if (required_frames ==0){
            break;
        }
    }

    if (startSearch == 1) {
        nFreeFrames -= _n_frames;
        return frame_no;
    } else {
        Console::puts("Unable to find a free frame for the given length :");Console::puti(_n_frames);Console::puts("\n");
        return 0;
    }
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    
    if (_base_frame_no < base_frame_no || base_frame_no + nframes < _base_frame_no + _n_frames) {
        Console::puts("The given frame range is out of the maximum possible frame range available and thus cannot be marked as inaccessible\n");
    } else {
        nFreeFrames -= _n_frames;
        int diff_between_bits = (_base_frame_no - base_frame_no)*2;
        int index_Of_Bitmap = diff_between_bits / 8;
        int index_Within_char = (diff_between_bits % 8) /2;

        int frames_required = _n_frames;

        unsigned char primary_mask = 0x80;
        unsigned char secondary_mask = 0xC0;
        primary_mask = primary_mask>>(index_Within_char*2);
        secondary_mask = secondary_mask>>(index_Within_char*2);
        while(frames_required > 0 && index_Within_char < 4) {
            bitmap[index_Of_Bitmap] = (bitmap[index_Of_Bitmap] & ~secondary_mask) | primary_mask; 
            primary_mask = primary_mask>>2;
            secondary_mask = secondary_mask>>2;
            frames_required--;
            index_Within_char++;
        }

        for(int i = index_Of_Bitmap + 1; i < index_Of_Bitmap + _n_frames/4; i++) {
            primary_mask = 0xC0;
            secondary_mask = 0xC0;
            for (int j = 0; j< 4 ; j++) {
                if (frames_required == 0) {
                    break;
                }
                bitmap[i] = (bitmap[i] & ~secondary_mask)| primary_mask;
                primary_mask = primary_mask>>2;
                secondary_mask = secondary_mask>>2;
                frames_required--;
            }
            if (frames_required ==0){
                break;
            }
        }
    }
}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
//First, we need to find the pool to which the current frame belongs to
    ContFramePool* present_frame_pool = ContFramePool::framepool_head;
    int i = 0;
    while ( (present_frame_pool->base_frame_no > _first_frame_no || present_frame_pool->base_frame_no + present_frame_pool->nframes <= _first_frame_no) ) {
        if (present_frame_pool->framepool_next == NULL) {
            Console::puts("Unable to find the frame in any of the present pools, cannot \n");
            return;
        } else {
            present_frame_pool = present_frame_pool->framepool_next;
        }
        i++;
    }

    unsigned char* current_FramePool_bitmap = present_frame_pool->bitmap;
    int diff_between_bits = (_first_frame_no - present_frame_pool->base_frame_no)*2;
    int index_Of_Bitmap = diff_between_bits / 8;
    int index_Within_char = (diff_between_bits % 8) /2;

    unsigned char primary_mask = 0x80;
    unsigned char secondary_mask = 0xC0;
    int bitmasker = 1;
    primary_mask = primary_mask>>index_Within_char*2;
    secondary_mask = secondary_mask>>index_Within_char*2;
    if (((current_FramePool_bitmap[index_Of_Bitmap]^primary_mask)&secondary_mask ) == secondary_mask) {
        current_FramePool_bitmap[index_Of_Bitmap] = current_FramePool_bitmap[index_Of_Bitmap] & (~secondary_mask); 
        index_Within_char++;
        bitmasker++;
        secondary_mask = secondary_mask>>2;
        present_frame_pool->nFreeFrames++;

        while (index_Within_char < 4) {
            if ((current_FramePool_bitmap[index_Of_Bitmap] & secondary_mask) == secondary_mask) {
                current_FramePool_bitmap[index_Of_Bitmap] = current_FramePool_bitmap[index_Of_Bitmap] & (~secondary_mask);
                index_Within_char++;
                secondary_mask = secondary_mask>>2;
                present_frame_pool->nFreeFrames++;
                bitmasker--;
            } else {
                return; 
            }
        } 

        for(int i = index_Of_Bitmap+1; i < (present_frame_pool->base_frame_no + present_frame_pool->nframes)/4; i++ ) {
            secondary_mask = 0xC0;
            for (int j = 0; j < 4 ;j++) {
                if ((current_FramePool_bitmap[i] & secondary_mask) == secondary_mask) {
                    current_FramePool_bitmap[i] = current_FramePool_bitmap[i] & (~secondary_mask);
                    bitmasker++;
                    secondary_mask = secondary_mask>>2;
                    present_frame_pool->nFreeFrames++;
                } else {
                    return; 
                }
            }
        } 


    } else {
        Console::puts("Cannot release this frame, it is not a head\n");
    }
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    return (_n_frames*2)/(8*4 KB) + ((_n_frames*2) % (8*4 KB) > 0 ? 1 : 0);
}
