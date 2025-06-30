//dummy follower object using a mover with a type higher than like 10 so i can follow the ball with a floating text object and it doesn't crash

function getAbilityOffset(ability) {
    return ((2*ability)*4) + 0x1D4;
}

//ignore all the notes trash talking my createBall function because it works without crashing now

////lowkey this function and createBall are pretty risky because i lowkey keep getting access violations after a while (hold on wait i might have the solution!)
////yeah don't use this function or reloadGun2 because i feel like they are 5x more likely to crash than just using Board's method lol
//function reloadGun() { //i haven't ACTUALLY seen how they reload the gun so this is my take on it
//    if (!Gun.ball) {
//        const sonic = createBall(false, false, 100, 100, 0, 0); //the reason we don't pass the first parameter as 1 (anchored) here is because if you do that it also sets the phys type to PHYS_PEG instead of PHYS_BALL and it doesn't like that (maybe because of the cmp and jne at offset 0x0004DA31)
//        sonic.anchored = 1; //anyways we just set anchored ourselves here
//        //technically when the ball is attached to the gun, its refCount is one (because the gun owns it) so im setting it to one too because when you shoot the ball it decrements the ref count and something weird will happen if i don't follow the rules (i can't be bothered to find out what actually happens in this situation)
//        sonic.refCount = 1; //owned by gun (which would happen automatically if i was doing this correctly)
//        Gun.ball = sonic; //BOOM
//        //you can't shoot the gun while the game is going but im cooking up a solution for that literally right as im writing this
//        canShootBallDuringTurns = true; //now you can
//    } else {
//        print("gun already had a ball so uhhhh im doing nothing");
//        //print(sonic);
//    }
//}
//
////here's a more accurate version of reloadGun (honestly i feel like this and the Board function might still cause access violations lol nothing in each function seemed to be different than what i was doing)
//function reloadGun2() {
//    if (!Gun.ball) {
//        const sonic = createBall(false, false, 100, 100, 0, 0);
//        sonic.anchored = 1;
//        //the Gun setNewBall function actually increments the refCount
//        Gun.setNewBall(sonic);
//        canShootBallDuringTurns = true; //now you can
//    } else {
//        print("gun already had a ball so uhhhh im doing nothing");
//    }
//}

onInit(() => { //runs on every file change or whenever you start a new level (which also triggers a file read)
    print("doin' ya mam doin' doin' ya mam");
    print("hit Control+D for le options");
    choice = 0;
    reactivate = false;
    grow = false;
    sigma = false;
    nuke = false;
});


onBallCountModify((eax, amountToAdd, edx, ballCount) => { //runs every time you gain or lose a ball in any way (besides like manually changing the value)
    return amountToAdd;
});

onPegHit((currentBall, physObj2, bool) => { //runs every time any ball hits a peg (even those hit by a multiball) 
    const pinfo = physObj2.pegInfo;

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

    if (sigma) {
        if (!pinfo.hit) {
            //sound id, and then another parameter that idk about lol
            SoundMgr.playSoundSimple(Math.floor(Math.random() * 76), 1);
        }
        const floatingText = LogicMgr.spawnFloatingText("~SIGMA~", currentBall.x, currentBall.y, 0x30); //the last param is the graphics type or something and it's usually 0x2D but for the big title stuff it uses 0x30
        floatingText.number = 0xA; //not sure if required
        floatingText.velRotation = .05;
        floatingText.rotation = .1; //you have to change the rotation for velRotation to kick in
        floatingText.color = Math.random()*255 << 16 | Math.random()*255 << 8 | Math.random()*255;
    }

    if (!Gun.ball) {
        //i forgot i had this on bruh OOPS!
        //Gun.ball = physObj2;
    }

    if (nuke) {
        physObj2.collision = false; //just a lil sumn sumn
    }

    //lowkey this worked for a second until boom access violation (but it was in a strange spot...)
    //i think i just found out why it crashed
    //i just did the same thing (except i only used one) and the moment the ball left the screen it crashed lmao (im assuming that once the turn is over, the ball is deleted and fucks up my trolling)
    //FloatingTextMgr.enumFloatingTextObjs((floatingText) => {
    //    //you gotta set the position of the text to the ball so it actually follows AT the ball
    //    floatingText.x = currentBall.x;
    //    floatingText.y = currentBall.y;
    //    floatingText.followingObj = currentBall;
    //});
});

onKeyDown((key, unused2, unused3) => { //runs every time you press a key while looking at the peggle window
    print("key -> " + key);
    if (key == 'T'.charCodeAt(0)) {
        //teleport the ball to our mouse!
        //and nevermind there's no function that lets me do that yet :(
        //yeah i've added a GetMousePos function now lol

        //well shoot now that i've added the WidgetManager you can just get the mouse position from there i suppose
        Board.enumBalls(ball => {
            //const ball = Board.ball;
            ball.setPosition(WidgetManager.mouseX - Board.viewX, WidgetManager.mouseY - Board.viewY);
            ball.setVelocity(0, 0);
        });

        /*
        const { x, y } = GetMousePos();
        //hmmm i need to get the peggle window for the mouse coords to be in screen space...
        print(x, y);
        //print(GetGUIThreadInfo(0)); //NULL
        const { hwndActive } = GetGUIThreadInfo(0);
        const rect = GetWindowRect(hwndActive);
        //print(rect.left, rect.left-x);
        //print(rect.top, rect.top-y);
        const ball = Board.ball;

        ball.x = x - (rect.left + Board.viewX); //77)
        ball.y = y - (rect.top + Board.viewY); //52)
        //hmm i still can't set the velocity because i lowkey don't know where they keep that at (just kidding i found it)
        ball.velX = 0;
        ball.velY = 0;
        */
    } else if (key == "M".charCodeAt(0)) {
        Board.enumBalls(ball => {
            //const ball = Board.ball;
            const pos = ball.getPosition();
            const mouse = { x: WidgetManager.mouseX - Board.viewX, y: WidgetManager.mouseY - Board.viewY };
            //hold on lemme normalize this vector (how do i do that again

            const vec = { x: mouse.x - pos.x, y: mouse.y - pos.y };
            const magnitude = Math.sqrt(vec.x ** 2 + vec.y ** 2);
            //wait a second this isn't what i wanted to do
            //all i wanted to do was add it to the current velocity lol
            //const newMagnitude = 1000 / magnitude; //ahh see now you get that attraction
            const newMagnitude = 1;

            //hold on, new idea
            //const newMagnitude = magnitude - (1 / magnitude); //hmmm how did somehow emulate what i was doing before lol
            const diff = magnitude / newMagnitude;
            vec.x /= diff;
            vec.y /= diff;
            //ball.setVelocity(vec.x, vec.y);
            ball.setVelocity(ball.velX + vec.x, ball.velY + vec.y);
        });
    } else if (key == 'R'.charCodeAt(0)) {
        //reloadGun();
        //nah we're using Board's function now lol
        const ball = Board.reloadGun(); //don't worry about calling reloadGun again if there's already a ball there, the game will automatically delete the old one!
        print(ball);
        canShootBallDuringTurns = true; //wink
        //reloadGun2();
    } else if (key == 'N'.charCodeAt(0)) { //n for nuke
        //this lowkey crashes every time i do it
        SoundMgr.playSoundSimple(SOUND_EXPLODE, 1);
        for (let i = 0; i < 100; i++) { //6*90 = 540 which is probably the width idk
            const pos = { x: 20 + (i * 6), y: 20 }; //haha weird when i spaced the balls out it would crash every time but when i didn't, it would actually run for like 30 seconds before it was over
            createBall(false, true, pos.x, pos.y, 0, 5);
        }
        nuke = true;
    } else if (key == 'C'.charCodeAt(0)) {
        const mouse = { x: WidgetManager.mouseX - Board.viewX, y: WidgetManager.mouseY - Board.viewY };
        let sonic;
        if (GetKey(0x10)) {
            sonic = createBall(true, true, mouse.x, mouse.y, 0.0, 0.0);
            sonic.imgname = "pinwheel"; //haha this workedf!
            //sonic.imgTextObj.text = "pinwheel";
            sonic.loadImageIfValidPath(); //uhhhh (no way)
            //next lets give it a mover!
            const mover = createMover(sonic, mouse.x, mouse.y);
            //negative movement flips the direction!
            mover.type = -MOVEMENT_VERTICAL_HORIZONTAL; //MOVEMENT_ROTATE_AT_ORIGIN; //wait a second pegs can't rotate!
            //amplitude doesn't effect rotate at origin (nevermind let's just use something else)
            mover.amplitude = 50;
            mover.speedDivisor = 0x320; //kinda slow
            //mover.originX = mouse.x;
            //mover.originY = mouse.y;
        } else if (GetKey(0x11)) {
            //oh wait i don't add this ball to the LogicMgr's peg arrays (i should probably do that)
            sonic = createBall(true, true, mouse.x, mouse.y, 0.0, 0.0);
            sonic.radius = 10.0; //default peg radius
            //let'\s try to create an actual peg
            const pinfo = createPegInfo(sonic, Math.floor(Math.random() * PEG_GREEN) + 1, 1);
        } else {
            sonic = createBall(false, true, mouse.x, mouse.y, -0.5, -3.0);
        }
        print(sonic);
    } else if (key == 'S'.charCodeAt(0)) {
        const ball = Board.reloadGun(); //don't worry about calling reloadGun again if there's already a ball there, the game will automatically delete the old one!
        canShootBallDuringTurns = true;
        LogicMgr.playSoundAndShootBall = 1;
    } /*else if (key == 'O'.charCodeAt(0)) {
        let hole;
        Board.enumPhysObjs(phys => {
            hole = phys;
            return true;
        });
        hole.mover.set_float(0x54, 326);
    }*/
    else if (key == 'D'.charCodeAt(0) && GetKey(0x11)) { //VK_CONTROL (0x11)
        SetForegroundWindow(GetConsoleWindow());
        print("#############");
        print("0: normal");
        print("1: every peg is purple");
        print("2: every peg is green");
        print("3: every peg is green AND the powerup is random");
        //print("4: activate random ability");
        print("4: every peg is green AND the powerup is random (excluding warren lol)");
        print(`5: [TOGGLE] pegs can be hit multiple times [${reactivate ? "ON" : "OFF"}]`);
        print(`6: [TOGGLE] ball grows with each peg hit [${grow ? "ON" : "OFF"}]`);
        print("7: set current ability");
        print("8: set ball count");
        print("9: set yellow text");
        //print("10: change peggle master"); //aw damn it doesn't work like that
        print(`10: [TOGGLE] sigma [${sigma ? "ON" : "OFF"}]`);
        print("11: set ball gravity");
        print(`12: [TOGGLE] slow motion [${Board.slowmotion ? "ON" : "OFF"}]`);
        print(`13: scramble gamble`);
        print(`14: set fast forward speed`);
        print(`15: [TOGGLE] allow fast forwarding during turns [${canFastForwardDuringTurns ? "ON" : "OFF"}]`);
        print(`16: enable secret movers`);
        print(`17: reactivate all pegs`);
        print(`18: reactivate all objects (tweaking)`);
        
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
            LogicMgr.yellowTextObj.text = getline("type the text you want to show -> ");
        } else if (response == 10) {
            //LogicMgr.currentEvent = LOGIC_EVENT_OPEN_CHARACTER_SELECT; //oops forgot to define that
            //LogicMgr.currentEvent = 8;

            sigma = true;
        } else if (response == 11) {
            gravity = parseFloat(getline("type a number (default gravity is 0.05) -> "));
        } else if (response == 12) {
            //Board.slowmotion = !Board.slowmotion;
            //if (Board.slowmotion) {
            //    print("#############");
            //    print("the slow motion speed is a number between 0 (completely frozen) and 1000 (normal speed)");
            //    print("the default is like 100 or 250");
            //    Board.slowMotionSpeed = parseInt(getline("type how slow you want the game to go -> "));
            //} else {
            //    Board.frozen = false;
            //}

            //oh wait we don't have to do it manually, i just realized there's a function for it
            let speed = 0xFA; //0xFA is usually what they pass
            if (!Board.slowmotion) { //checking if it's not already on since we change it AFTER this if statement
                print("#############");
                print("the slow motion speed is a number between 0 (completely frozen) and 1000 (normal speed)");
                print("the default is like 100 or 250");
                //Board.slowMotionSpeed = parseInt(getline("type how slow you want the game to go -> "));
                speed = parseInt(getline("type how slow you want the game to go -> "));
            }
            Board.setSlowMotion(!Board.slowmotion, speed);
        } else if (response == 13) {
            defaultTextColor = Math.random() * (2 ** 24);
            defaultBallRadius = Math.random() * 16;
            gravity = Math.random() / 10;
            SoundMgr.playSoundSimple(Math.floor(Math.random() * 76), 1);
        } else if (response == 14) {
            print("#############");
            print("the default fast forward speed is 8 and the highest value you can put is 127 (nothing happens after 127)");
            fastForwardSpeed = parseInt(getline("type how fast you can fast forward -> "));
        } else if (response == 15) {
            canFastForwardDuringTurns = !canFastForwardDuringTurns;
        } else if (response == 16) {
            Board.enumPhysObjs(obj => {
                obj.enableMover = 1;
            });
        } else if (response == 17) {
            LogicMgr.enumPegs(obj => {
                obj.setActive(true);
            });
        } else if (response == 18) {
            Board.enumPhysObjs(obj => {
                obj.setActive(true); //OH MY GOD WHAT
            });
        }
        else {
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