function getAbilityOffset(ability) {
    return ((2*ability)*4) + 0x1D4;
}

onInit(() => { //runs on every file change or whenever you start a new level (which also triggers a file read)
    print("doin' ya mam doin' doin' ya mam");
    choice = 0;
    reactivate = false;
    grow = false;
});


onBallCountModify((eax, amountToAdd, edx, ballCount) => { //runs every time you gain or lose a ball in any way (besides like manually changing the value)
    return amountToAdd;
});

onPegHit((currentBall, physObj2, bool) => { //runs every time any ball hits a peg (even those hit by a multiball) 
    const pinfo = physObj2.PegInfo;

    if (choice == 1 || choice == 2 || choice == 3 || choice == 4) {
        const peg_type = choice > 1 ? PEG_GREEN : PEG_PURPLE;
        if (pinfo.type == PEG_ORANGE) {
            //LogicMgr.orangePegsLeft--; //haha (wait i don't think it actually works like this)
            return;
        }
        pinfo.type = peg_type;
        if (choice == 3 || choice == 4) {
            LogicMgr.currentAbility = Math.floor(Math.random()*ABILITY_WARRENSPECIAL) + (choice == 3);
        }
    }

    if (reactivate) {
        pinfo.hit = false;
    }

    if (grow) {
        currentBall.radius += .1;
    }

    //lowkey this worked for a second until boom access violation (but it was in a strange spot...)
    //FloatingTextMgr.enumFloatingTextObjs((floatingText) => {
    //    floatingText.followingObj = currentBall;
    //});
});

onKeyDown((key, unused2, unused3) => { //runs every time you press a key while looking at the peggle window
    print("key -> " + key);
    if (key == 'T'.charCodeAt(0)) {
        //teleport the ball to our mouse!
        //and nevermind there's no function that lets me do that yet :(
        //yeah i've added a GetMousePos function now lol
        const { x, y } = GetMousePos();
        //hmmm i need to get the peggle window for the mouse coords to be in screen space...
        print(x, y);
        //print(GetGUIThreadInfo(0)); //NULL
        const { hwndActive } = GetGUIThreadInfo(0);
        const rect = GetWindowRect(hwndActive);
        //print(rect.left, rect.left-x);
        //print(rect.top, rect.top-y);
        const ball = Board.Ball;

        ball.x = x - (rect.left + 77);
        ball.y = y - (rect.top + 52);
        //hmm i still can't set the velocity because i lowkey don't know where they keep that at (just kidding i found it)
        ball.velX = 0;
        ball.velY = 0;
    } else if (key == 'D'.charCodeAt(0) && GetKey(0x11)) { //VK_CONTROL (0x11)
        SetForegroundWindow(GetConsoleWindow());
        print("#############");
        print("0: normal");
        print("1: every peg is purple");
        print("2: every peg is green");
        print("3: every peg is green AND the powerup is random");
        //print("4: activate random ability");
        print("4: every peg is green AND the powerup is random (excluding warren lol)");
        print("5: [TOGGLE] pegs can be hit multiple times");
        print("6: [TOGGLE] ball grows with each peg hit");
        print("7: set current ability");
        print("8: set ball count");
        print("9: set yellow text");
        //print("10: change peggle master"); //aw damn it doesn't work like that
        
        const response = parseInt(getline("type your answer nigga -> "));
        if (response == 5) {
            reactivate = !reactivate;
        } else if (response == 6) {
            grow = !grow;
        } else if (response == 7) {
            print("#############");
            print("0: do nothing lol");
            print("1: super guide");
            print("2: flippers");
            print("3: multiball");
            print("4: pyramid");
            print("5: space blast");
            print("6: spooky ball");
            print("7: zen shot");
            print("8: shot extender (hidden ability!)"); //https://www.youtube.com/watch?v=ny2-H9TaAqk
            print("9: flower power");
            print("10: nudge (hidden ability!!! hold click to activate)"); //https://www.youtube.com/watch?v=VGCai8uCNZc
            print("11: timebomb (hidden ability!!! click to activate)"); //https://www.youtube.com/watch?v=VGCai8uCNZc
            print("12: fireball");
            print("13: magic wheel");
            const res = parseInt(getline("choose one nigga -> "));
            if (res) {
                LogicMgr.currentAbility = res;
            }
        } else if (response == 8) {
            LogicMgr.balls = parseInt(getline("type how many balls you want nigga -> "));
        } else if (response == 9) {
            LogicMgr.textObject.text = getline("type the text you want to show -> ");
        } else if (response == 10) {
            //LogicMgr.currentEvent = LOGIC_EVENT_OPEN_CHARACTER_SELECT; //oops forgot to define that
            //LogicMgr.currentEvent = 8;
        } else {
            choice = response;
            if (choice == 0) {
                grow = false;
                reactivate = false;
            }
        }
        //if (choice == 4) {
        //    //haha this might break stuff
        //    LogicMgr.activate_ability(false, false, );
        //}
    }
}, true);