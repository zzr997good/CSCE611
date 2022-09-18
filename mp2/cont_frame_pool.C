/*
 File: ContFramePool.C
 
 Author:
 Date  :
 
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
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

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

ContFramePool* ContFramePool::head;
ContFramePool* ContFramePool::tail;

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool::FrameState ContFramePool::get_state(unsigned long _frame_no) {
    unsigned char val ;
    unsigned int bitmap_index = _frame_no / 4;
    unsigned int inner_index = _frame_no % 4;
    if(inner_index == 0)  val = bitmap[bitmap_index].bmp0;
    if(inner_index == 1)  val = bitmap[bitmap_index].bmp1;
    if(inner_index == 2)  val = bitmap[bitmap_index].bmp2;
    if(inner_index == 3)  val = bitmap[bitmap_index].bmp3;
    FrameState state;
    switch (val) {
        case 0x0:
            state=FrameState::Free;
            break;
        case 0x1:
            state=FrameState::HoS;
            break;
        case 0x2:
            state=FrameState::Inacs;
            break;
        case 0x3:
            state=FrameState::Used;
            break;
    }
    return state;
    
}

void ContFramePool::set_state(unsigned long _frame_no, FrameState _state) {
    unsigned int bitmap_index = _frame_no / 4;
    unsigned int inner_index = _frame_no % 4;
    unsigned char val;
    switch (_state) {
        case FrameState::Free:
            val=0x0;
            break;
        case FrameState::HoS:
            val=0x1;
            break;
        case FrameState::Inacs:
            val=0x2;
            break;
        case FrameState::Used:
            val=0x3;
            break;
    }
    if(inner_index == 0)  bitmap[bitmap_index].bmp0=val;;
    if(inner_index == 1)  bitmap[bitmap_index].bmp1=val;
    if(inner_index == 2)  bitmap[bitmap_index].bmp2=val;
    if(inner_index == 3)  bitmap[bitmap_index].bmp3=val;
    
}


ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no)
{
    //  Console::puts("ContframePool::Constructor not implemented!\n");
    //  assert(false);
    base_frame_no = _base_frame_no;
    n_frames = _n_frames;
    nFreeFrames = _n_frames;
    info_frame_no = _info_frame_no;
    
    //If this is a kernel pool, info_frame_no==0 and the first frame of kernel pool should save the kernel pool info
    if(info_frame_no == 0)
    {
        bitmap = (bitmap_char_s *) (base_frame_no * FRAME_SIZE);
    }
    //If this is a process pool, info_frame_no!=0 and one allocated frame of kernel pool should save the process pool info
    else
    {
        bitmap = (bitmap_char_s *) (info_frame_no * FRAME_SIZE);
    }
    
    for(int fno = 0; fno < _n_frames; fno++) {
        set_state(fno, FrameState::Free);
    }
    
    //If info_frame_no==0, the pool info is stored in the first frame of pool, which always happens in kernel pool
    //If info_frame_no!=0, the pool info is stored in the kernel pool, so all frames in process pool are free, which always happens in process pool
    if(info_frame_no == 0)  {
        set_state(0, FrameState::HoS);
        nFreeFrames--;
    }
    
    //Link this pool to the tail of pool list
    if(ContFramePool::head==NULL) {
        ContFramePool::head=this;
        ContFramePool::tail=this;
    }
    else{
        ContFramePool::tail->next=this;
        ContFramePool::tail=this;
    }
    next=NULL;
    Console::puts("ContframePool::Frame pool is initialized!\n");
}


unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    //    Console::puts("ContframePool::get_frames not implemented!\n");
    //    assert(false);
    assert(nFreeFrames >= _n_frames);
    unsigned int i = 0;
    unsigned int j = 0;
    bool searching = true;                      //Flag that search stops if consecutive frames are truncated by one used frame
    bool found = false;                                //Flag that find _n_frames or not
    while(i < n_frames-_n_frames+1)                    //avoid searching out of bounds
    {
        if(get_state(i) == FrameState::Free)           //i is the potential head of frame sequence
        {
            for(j = i+1; j < i+ _n_frames ; j++)           //check [i..i+_n_frames]
            {
                if(get_state(j)!=FrameState::Free)
                {
                    searching = false;
                    break;
                }
            }
            if(searching == true)                  //_n_frames free frames found
            {
                found = true;
                break;
            }
            if(searching == 0)                     //try to start from next frame
            {
                searching = true;
                i = j + 1;
            }
        }
        else
            i++;
    }
    
    if(found == true)
    {
        for(j = i; j < i+ _n_frames ; j++)                   //Set the appropriate head_of_sequence bit & clear the free bits
        {
            if(j == i)
                set_state(j, FrameState::HoS);
            else
                set_state(j, FrameState::Used);
        }
        nFreeFrames = nFreeFrames - _n_frames;              //Reduce the no of free frames
        Console::puts("ContframePool::getFrames() Frame sequence allocated!\n");
        return base_frame_no+i;
    }
    else
    {
        Console::puts("ContframePool::getFrames() Frame sequence could not be allocated!\n");
        return 0;                                           //Allocation request didn't go through ; return 0
    }
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    //    Console::puts("ContframePool::mark_inaccessible not implemented!\n");
    //    assert(false)
    assert ((_base_frame_no >= base_frame_no) && (_base_frame_no < base_frame_no + n_frames));
    assert ((_base_frame_no + _n_frames >= base_frame_no) && (_base_frame_no + _n_frames < base_frame_no + n_frames));
    for(unsigned long abs_fno = _base_frame_no; abs_fno < _base_frame_no + _n_frames; abs_fno++){
        set_state(abs_fno-base_frame_no, FrameState::Inacs);
        nFreeFrames--;                                                  //Reduce the free frames count
    }
    Console::puts("ContFramePool::mark_inaccessible - Memory marked inaccessigble\n");
}


void ContFramePool::release_from_my_pool(unsigned long _first_frame_no)
{
    //Make Sure this frame should be in the state of Used, instead of
    if(get_state(_first_frame_no-base_frame_no)!=FrameState::HoS){
        Console::puts("Frame not head of sequence in one pool, cannot release \n");
        return;
    }
    else{
        unsigned long i = _first_frame_no;
        //Release the head first
        set_state(i - base_frame_no,FrameState::Free);
        nFreeFrames++;
        i++;
        //Release frame one by one
        while(i-base_frame_no<n_frames&&get_state(i - base_frame_no)== FrameState::Used)
        {
            set_state(i - base_frame_no,FrameState::Free);     //release one frame
            nFreeFrames++;                  //Increase free count
            i++;
        }
        Console::puts("ContFramePool::release_frames - Frame sequence released\n");
    }
}


void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    
    //    Console::puts("ContframePool::release_frames not implemented!\n");
    //    assert(false);
    ContFramePool* cur = ContFramePool::head;
    while(cur != NULL)
    {
        if((_first_frame_no >= cur->base_frame_no) && (_first_frame_no < cur->base_frame_no + cur->n_frames))
        {
            //Framepool found
            break;
        }
        cur = cur->next;
    }
    
    //Cant find this frame in any pool
    if(cur==NULL){
        Console::puts("Frame not found in any pool, cannot release. \n");
        return;
    }
    //Release frames using the private release function of this pool management
    cur->release_from_my_pool(_first_frame_no);
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    
    //    Console::puts("ContframePool::need_info_frames not implemented!\n");
    //    assert(false);
    
    //1 byte bitmap can represent 4 frames
    //1 frame_size bitmap can represent 4*frame_size frames
    return (_n_frames+4*FRAME_SIZE-1)/(4*FRAME_SIZE);
}
