#ifndef UART_FRAME_H
#define UART_FRAME_H

/*
 * Wire format shared by both ends of the link (Pico 2W <-> Zero 2W).
 *
 * Every frame is a FIXED 24 bytes:
 *
 *   byte 0                                              byte 23
 *   ( <........... up to 22 bytes of content + pad ...........> )
 *   ^ FRAME_SOF                                         FRAME_EOF ^
 *
 * - SOF is ALWAYS at byte 0  -> single-compare framing-loss check.
 * - EOF is ALWAYS at byte 23 (content padded inward with FRAME_PAD).
 * - The CPU scans the buffer for FRAME_EOF and stops; trailing pad
 *   bytes are never acted on.
 *
 * Keeping the size fixed is what lets the RX DMA transfer count stay
 * static (24) so the ping-pong re-arm never has to recompute it.
 */

#define UART_FRAME_SIZE   24u
#define FRAME_SOF         '('
#define FRAME_EOF         ')'
#define FRAME_PAD         '-'

#endif /* UART_FRAME_H */
