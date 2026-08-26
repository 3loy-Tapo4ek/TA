integer solve()
start
    mutable integer can_move := 0;

    can_move := right;
    checkzero (can_move)
    start

    finish
    instead
    start
        call solve with [];
        timeshift 1;
    finish

    can_move := bottom;
    checkzero (can_move)
    start

    finish
    instead
    start
        call solve with [];
        timeshift 1;
    finish


    can_move := left;
    checkzero (can_move)
    start

    finish
    instead
    start
        call solve with [];
        timeshift 1;
    finish


    can_move := top;
    checkzero (can_move)
    start

    finish
    instead
    start
        call solve with [];
        timeshift 1;
    finish

    return 0;
finish


start
    call solve with [];
finish