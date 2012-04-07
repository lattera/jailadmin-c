#if !defined(_JAILADMIN_INTERACTIVE_H)
#define _JAILADMIN_INTERACTIVE_H

#define MAINWIN 0
#define INPUTWIN 1
#define NUMWINS 2

typedef enum _interactive_callback_return { INVALID=0, CONT_PROC=1, TERM_PROC=2 } CALLBACK_RETURN;

/* Arguments: pointer to JAILADMIN object, pointer to parsed command, pointer to ncurses window */
typedef CALLBACK_RETURN (*interactive_callback)(JAILADMIN *, char **);

void interactive(JAILADMIN *);

#endif
