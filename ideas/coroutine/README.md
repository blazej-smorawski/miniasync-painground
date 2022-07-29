# coroutine #
While using miniasync, I found out that future chains are boring(and waste memory), so I wanted
to make something that would simplify the API and make it easier to for ex. return errors.

The whole idea is based on a *switch* that takes the future's stage into account, so it can
redirect *future_poll* to an appropriate *subroutine*
```c
COROUTINE
{
    printf("Stage 0\n");
    int x = 5;
}
SUBROUTINE(subroutine, subroutine(3))
{
    printf("Stage 1\n");
}
SUBROUTINE(subroutine, subroutine(1))
{
    printf("Stage 2\n");
}
SUBROUTINE(subroutine, subroutine(5))
END_COROUTINE
```

Looking at the API, you can see that it's wrapped around *switch*, so there's nothing
fascinating, and we cannot do *if* clauses etc.