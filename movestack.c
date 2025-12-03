void
movestack(const Arg *arg)
{
    Client *c = NULL, *p = NULL, *pc = NULL, *i;

    if (!selmon->sel)
        return;

    if (arg->i > 0) {
        /* move up in stack */
        for (c = selmon->sel; c && (c = c->next);)
            if (!c->isfloating && ISVISIBLE(c))
                break;
        if (!c)
            for (c = selmon->clients; c && c != selmon->sel; c = c->next)
                if (!c->isfloating && ISVISIBLE(c))
                    break;
    } else {
        /* move down in stack */
        for (i = selmon->clients; i != selmon->sel; i = i->next)
            if (!i->isfloating && ISVISIBLE(i))
                c = i;
        if (!c)
            for (; i; i = i->next)
                if (!i->isfloating && ISVISIBLE(i))
                    c = i;
    }

    /* find previous client */
    for (p = selmon->clients; p && p->next != c; p = p->next)
        ;
    /* find previous selected client */
    for (pc = selmon->clients; pc && pc->next != selmon->sel; pc = pc->next)
        ;

    if (c && c != selmon->sel) {
        /* attach c where sel was */
        if (pc)
            pc->next = c;
        else
            selmon->clients = c;

        if (p)
            p->next = selmon->sel;
        else
            selmon->clients = selmon->sel;

        i = selmon->sel->next;
        selmon->sel->next = c->next;
        c->next = i;

        arrange(selmon);
    }
}
