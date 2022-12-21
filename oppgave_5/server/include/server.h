#ifndef __CPROG_CLIENT
#define __CPROG_CLIENT

/*
 * Binds and listens to socket.
 *
 * Returns a socket file descriptor on success, -1 on failure.
 */
int BindAndListen();
/*
 * Fires when user hits CTRL+C to exit. Closes the socket and sets iRunning to 0.
 */
void SIGINTHandler();
/*
 * Fires on error. Closes the socket and sets iRunning to 0.
 */
void SIGABRTHandler();

#endif
