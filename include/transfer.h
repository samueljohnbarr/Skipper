#ifndef TRANSFER_H_
#define TRANSFER_H_


/**
 * Transfers an object by grabbing, lifting,
 * turning the bot on its axis, then setting the object back down.
 * Robot returns back to orgininal position when process is complete
 */
void objectTransfer();

/**
 * Returns if an object is in range to grab
 */
 int isInRange();

#endif //TRANSFER_H_
