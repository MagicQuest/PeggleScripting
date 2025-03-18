/*print("yo yo yo what it is my niggas");
print("doin' ya mam doin' doin' ya mam");
function both(...args) {
    print(...args);
    OutputDebugString(...args);
}

both(globalThis);
both(eax);
both(ecx);
both(edx);
both(ballCount); //ball game...

return 21;*/

function both(...args) {
    print(...args);
    OutputDebugString(...args);
}

function getAbilityOffset(ability) {
    return ((2*ability)*4) + 0x1D4;
}

function findFirstPeg() { //oops this still includes pegs that were hit
    let peg = undefined;
    Board.enumPhysObjs((obj) => {
        if (obj.name.includes("Brick") || obj.name.includes("Ball")) {
            if (obj.PegInfo) {
                peg = obj;
                return true;
            }
        }
    });
    return peg;
}

function findFirstUntouchedPeg() {
    let peg = undefined;
    Board.enumPhysObjs((obj) => {
        if (obj.name.includes("Brick") || obj.name.includes("Ball")) {
            let pinfo;
            if ((pinfo = obj.PegInfo) && !pinfo.hit) {
                peg = obj;
                return true;
            }
        }
    });
    return peg;
}

function findFirstMovingPeg() {
    let peg = undefined;
    Board.enumPhysObjs((obj) => {
        if (obj.name.includes("Brick") || obj.name.includes("Ball")) {
            let pinfo;
            if ((pinfo = obj.PegInfo) && !pinfo.hit && obj.Mover) {
                peg = obj;
                return true;
            }
        }
    });
    return peg;
}

onInit(() => { //runs on every file change or whenever you start a new level (which also triggers a file read)
    both("doin' ya mam doin' doin' ya mam");

    //do init stuff for global variables if you have any
    pegs = []; //initialize de global
    lastHit = undefined;

    //LogicMgr.currentAbility = ABILITY_MULTIBALL;
    both(version());
});


onBallCountModify((eax, amountToAdd, edx, ballCount) => { //runs every time you gain or lose a ball in any way (besides like manually changing the value)
    both("onBallCountModify!");
    both(eax);
    both(amountToAdd);
    both(edx);
    both(ballCount);
    both("end!");
    return amountToAdd;
});

onPegHit((currentBall, physObj2, bool) => { //runs every time any ball hits a peg (even those hit by a multiball)
    //i think physObj2 can either be a brick or a ball?
    //no way...
    //the Ball class doubles as a peg!
    //there's a brick class for the square pegs but they reuse the ball class for pegs too! that's why they both have a pointer to a PegInfo object
    //ok so wait does that mean i could set the current ball pointer to the physObj2 and it might not crash? (holy moly)
    //i just made it so that you could do that

    //print(currentBall); //i think the first parameter is the same ball that Board has
    //print(currentBall.location == Board.Ball.location); //is true right>? yes!
    //print(physObj2); //sometimes brick (depending on what kind of peg you hit, circle == ball, square == brick)
    lastHit = physObj2;
    const pinfo = physObj2.pegInfo;
    if (!pinfo.hit) {
        pegs.push(pinfo);
        print("bpushed");
    }
    //print(physObj2);
    //physObj2.PegInfo.type++; //LKMAO! (ok i crashed that shit)
    /*if (pinfo.type < 4) { //4 is the max type i think (PEG_GREEN)
        pinfo.type++;
    }*/

    //Board.Ball = physObj2; //aw damn nothing too cool happened (the ball and the peg disappeared but the ball was still hitting pegs)
    //wait hold on i should test if setting the peg info does anything special
    //Board.Ball.PegInfo = physObj2.PegInfo; //lmao it looks kind of cool because it shows the peg effects on the ball but it will crash the next time you shoot (so not worth it)
    //wait hold on i should see if anything happens if i set the image at 0x24C
    //Board.Ball.set_dword(0x24C, physObj2.get_dword(0x24C));

    //LogicMgr.currentAbility = Math.floor(Math.random() * ABILITY_WARRENSPECIAL); //THIS IS GOATED!

    //idk if this function does anything more special than just setting the type yourself...
    //physObj2.set_peginfo_type(PEG_PURPLE); //they use this function to move the purple peg around after every shot
    if (pinfo.type != PEG_ORANGE && pinfo.type != PEG_GREEN) { //actually i could just change the amount of orange pegs left and still do the effect too
        //if (pegs.length == 1) {
            //physObj2.set_peginfo_type(PEG_GREEN);
        //}
        //pinfo.type = Math.floor(Math.random()*PEG_GREEN)+1;
    }

    const floatingText = LogicMgr.spawnFloatingText("~SIGMA~", currentBall.x, currentBall.y, 0x2D);
    floatingText.velRotation = .05;
    floatingText.rotation = .1; //you have to change the rotation for velRotation to kick in

    //print(Math.floor(Math.random() * ABILITY_WARRENSPECIAL)+1);

    //OH NO!
    //space blast hits all the pegs around it so when i activate it, it calls PegHit for each of those pegs!
    //now we're calling RunEvent from this thread instead of the main one and it's getting totally fucked!
    //never mind my boys i've fixed it but the down side is that we can't handle the PegHit event for those on the JS side
    //so i guess if you want to activate space blast (and get the peghit events) you might have to do the technique of setting your current ability to space blast and then setting the peg type to be green like i did earlier lol

    //const goodpowers = [ABILITY_SUPERGUIDE, ABILITY_FLIPPERS, ABILITY_MULTIBALL, ABILITY_PYRAMID, ABILITY_SPOOKYBALL, ABILITY_ZENSHOT, ABILITY_SHOTEXTENDER, ABILITY_FIREBALL]

    //LogicMgr.activate_ability(currentBall, physObj2, goodpowers[Math.floor(Math.random()*goodpowers.length)], 1);

    //LogicMgr.activate_ability(currentBall, physObj2, Math.floor(Math.random() * (ABILITY_WARRENSPECIAL-1)) + 1, 1); //uh oh does space blast do something special because why is it hanging my thread

    //LogicMgr.activate_ability(currentBall, physObj2, 5, 1); //uh oh does space blast do something special because why is it hanging my thread

    //physObj2.x = currentBall.x;
    //physObj2.y += 10;
    //physObj2.maybeY = currentBall.y - 20;

    /*if (!pinfo.hit) {
        currentBall.velX *= 10;
        currentBall.velY *= 10;
        //lol i haven't added the velocity for the version i got loaded so i gotta do it the oldfasioned way
        //currentBall.set_float(0xFC, currentBall.get_float(0xFC) * 10);
        //currentBall.set_float(0x100, currentBall.get_float(0x100) * 10);
    } else {
        currentBall.velX *= .25;
        currentBall.velY *= .25;
        //currentBall.set_float(0xFC, currentBall.get_float(0xFC) * .25);
        //currentBall.set_float(0x100, currentBall.get_float(0x100) * .25);
    }*/

    //pinfo.set_peg_hit(false);

    //pinfo.type = PEG_PURPLE;
    //pinfo.hit = 0;
    //both(bool);
    //print(LogicMgr);
});

onKeyDown((key, unused2, unused3) => { //runs every time you press a key while looking at the peggle window
    both("key -> " + key);
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
        const ball = Board.ball;

        ball.x = x - (rect.left + 77);
        ball.y = y - (rect.top + 52);
        //hmm i still can't set the velocity because i lowkey don't know where they keep that at (just kidding i found it)
        ball.velX = 0;
        ball.velY = 0;
    } else if (key == 'P'.charCodeAt(0)) {
        //let allpegs = [];
        Board.enumPhysObjs((obj) => { //hell yeah
            //if (obj.name.includes("Ball") || obj.name.includes("Brick")) {
            //    allpegs.push();
            //}
            //print(obj.name); //simple my boy
        });
        /*for (const peg of allpegs) {
            const pinfo = peg.PegInfo;
            if (pinfo) {
                
            }
        }*/
    }
});

/*onBallCountModify((eax, ecx, edx, ballCount) => { //return ecx for default behavior!
    ////both(eax);
    ////both(ecx);
    ////both(edx)
    //both(ballCount);
    //both((ballCount / 2) * ecx);
    ////let ret = Math.floor((ballCount / 2) * ecx);
    ////both(Math.trunc((ballCount / 2) * ecx));
    ////ret += ecx; //just in case
    ////both(ret);
    ////actual better one that's not rigged
    both(eax, ecx, edx, ballCount);
    //both(peggle);
    //both(globalThis);
    //peggle.balls = 0;// = Math.ceil(peggle.score/1000) || 9;
    both(LogicMgr.get_dword(0x1E4));
    //peggle.set_dword(0x1E4, 5); //flipper count!
    // peggle.zenCount = 2;
    both(LogicMgr.get_dword(0x20));
    //peggle.spookyCount = 5;
    both(LogicMgr.get_dword(getAbilityOffset(6)));

    //peggle.text = "MANGO";
    //peggle.textLength = 5;

    //peggle.activate_ability(peggle.ABILITY_SPOOKYBALL);

    // const currentAbility = peggle.get_dword(0x1C4);

    // peggle.set_dword(getAbilityOffset(currentAbility), 5);

    if (eax != 0) { //i just wanna know when eax ISN'T 0
        both("name one time eax wasn't 0");
        both("eax:", eax);
        __debugbreak();
    }

    let ret = ecx;
    if (ecx > 0) {
        //ret = (ballCount);
        ret *= 2;
        //Sleep(250 + (Math.random()*250));
    } else {
        //Sleep(500);
    }
    ret = ret || ecx;
    return ecx; //(ballCount/2)*ecx;
});*/

//Msgbox("mangophonk");