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

both("doin' ya mam doin' doin' ya mam");

function getAbilityOffset(ability) {
    return ((2*ability)*4) + 0x1D4;
}

onBallCountModify((eax, ecx, edx, ballCount) => { //return ecx for default behavior!
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
});

both(version());

//Msgbox("mangophonk");