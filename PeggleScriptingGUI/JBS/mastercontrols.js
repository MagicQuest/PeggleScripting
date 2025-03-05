//im not even gonna lie this needs a full rewrite it's SO bad just like jbsblueprints itself
//i actually should make a version of this in c++ because the types will guide the way i make the classes so they actually make sense
//i could also use typescript but it would be cool to have a full c++ equivalent of this ole thang (i did the bare minimum in PeggleScripting/PeggleScriptingGUI)

const Int_To_WM = function() { //this gotta be the only good reason to use a closure
    const wm = {}; //with a closure im using wm like it's static!!!
    Object.entries(globalThis).filter(([key, value]) => { //wait why am i doing a filter instead of just forEach
        let k = key.indexOf("WM_") == 0;
        k && (wm[value] = key); //since Object.entries returns stuff like [["WM_PAINT", 0], ["WM_CREATE", 1]] im just setting the values directly (also im swapping key and value here so it's easier to get the name of the message by the number)
        return k;
    });

    function Int_To_WM(msg) {
        return wm[msg];
    }
    return Int_To_WM;
}();

//GLManager.programs.push({name: "fortnite", vertexid: 0, fragmentid: 1}, {name: "fortnite", vertexid: 0, fragmentid: 1}  );

globalThis.withinBounds = function({x, y, width, height}, pair) {
    return pair.x > x && pair.y > y && pair.x < x+width && pair.y < y+height;
}

globalThis.hand = LoadCursor(NULL, IDC_HAND);
globalThis.arrow = LoadCursor(NULL, IDC_ARROW);

globalThis.TOP = 0b00000000001;
globalThis.LEFT = 0b00000000010;
globalThis.RIGHT = 0b00000000100;
globalThis.BOTTOM = 0b00000001000;
globalThis.BUTTON  = 0b00000010000;
globalThis.MOVE     = 0b00000100000;
globalThis.CONTEXTMENU=0b00001000000;
globalThis.DROP        =0b00010000000;
globalThis.DRAG         =0b00100000000;
globalThis.TEXT          =0b01000000000;
globalThis.WHEEL          =0b10000000000;

const hittestarr = [
    [TOP, LoadCursor(NULL, IDC_SIZENS)],
    [LEFT, LoadCursor(NULL, IDC_SIZEWE)],
    [RIGHT, LoadCursor(NULL, IDC_SIZEWE)],
    [BOTTOM, LoadCursor(NULL, IDC_SIZENS)],
    [BUTTON, hand],
    [MOVE, LoadCursor(NULL, IDC_SIZEALL)],
    [CONTEXTMENU, LoadCursor(NULL, IDC_HELP)],
    [DROP, hand],
    [DRAG, hand],
    [TEXT, LoadCursor(NULL, IDC_IBEAM)],
    [WHEEL, LoadCursor(NULL, 32652)], //idk why 32652 isn't named
];

function diyfuzzysort(value) {
    return function(a, b) {
        let bv = 0;
        let av = 0;
        for(let i = 0; i < value.length; i++) { //came up with this bad boy meself
            const snip = value.substring(0, i+1);
            bv += (b.toLowerCase()).includes(snip);
            av += (a.toLowerCase()).includes(snip);
        }
        return bv - av;
    }
}

globalThis.forAllArrays = function(callback, ...arrays) {
    for(let i = 0; i < arrays.length; i++) {
        const array = arrays[i];
        for(const o of array) {
            callback(o);
        }
    }
}

//function* multiplyChar(char, x) { //almost like python (wow these generator functions are kinda lit tho...)
//    for(let i = 0; i < x; i++) {
//        yield char;
//    }
//}

//wait a second these generator functions are a trick
function multiplyString(char, x) {
    let str = "";
    for(let i = 0; i < x; i++) {
        str += char;
    }
    return str;
}

globalThis.printPaneTree = function(pane, depth = 0) {
    print(`${multiplyString("    ", depth)}${pane.constructor.name} (${pane.alwaysHittest}) : {`);
    if(pane.children.length == 0) {
        print(multiplyString("    ", depth+1)+"(null)");
    }else {
        for(let i = 0; i < pane.children.length; i++) {
            const child = pane.children[i];
            printPaneTree(child, depth+1);
        }
    }
    print(multiplyString("    ", depth)+`}${depth!=0 ? "," : ";"}`);
}

class Draggable { //not to be confused with Draggable from jbstudio3 (this shit is elegant as fuck)
    static lockX = false;
    static lockY = false;
    static dragging = false;
    static dragged = undefined;
    static offset = undefined;
    static defaultRelease = true;

    static select(object, mouse, lx, ly, releaseonmouseup = true) {
        Draggable.dragging = true;
        Draggable.dragged = object;
        Draggable.offset = {x: mouse.x - object.x, y: mouse.y - object.y};
        Draggable.lockX = lx;
        Draggable.lockY = ly;
        Draggable.defaultRelease = releaseonmouseup;
        if(object != camera && object instanceof Blueprint) { //lol i want blueprints to still be selected when dragging the camera
            Blueprint.active = object; //apparently this is the ONLY time i set Blueprint.active which is kinda weird but i guess not that weird
            //if(activePane instanceof Blueprint) {
                PropertyMenu.addFunctionDetails(); //hell yeah borhter
            //}
        }
    }

    static update(mouse) {
        !Draggable.lockX && (Draggable.dragged.x = mouse.x-Draggable.offset.x);
        !Draggable.lockY && (Draggable.dragged.y = mouse.y-Draggable.offset.y);
        Draggable.dragged.onDrag?.();
        print(Draggable.dragged.x, Draggable.dragged.y, mouse.x, mouse.y, );
        dirty = true;
    }

    static release() {
        //Draggable.lockX = false;
        //Draggable.lockY = false;
        Draggable.dragging = false;
        Draggable.dragged = undefined;
        Draggable.offset = undefined;
    }

    static mouseUp() {
        if(Draggable.defaultRelease) {
            Draggable.release();
        }
    }
}

class Editable { //for text :smirk: (unfortunately you must draw the text and caret yourself)
    static caret = 0;
    static editing = false;
    static edited = undefined;
    static returntoquit = true;

    static ctrlDelimiters = [".", " "]; //points where using ctrl+backspace/delete will stop
    //static ctrlDelimiters = /[."]/;

    static beginInput(caret = 0, tobeedited, returntoquit = true) { //named like roblox's UserInputService (oh i just googled it and it's actually inputBegan im a FRAUD)
        if(Editable.editing && tobeedited != Editable.edited) Editable.endInput();
        Editable.caret = caret;
        Editable.edited = tobeedited;
        Editable.edited.value = Editable.edited.value?.toString() ?? "undefined"; //just in case lol
        Editable.editing = true;
        Editable.returntoquit = returntoquit;
        print("HELL YEAH BEGIN INPUT");
        print("value:", tobeedited.value);
    }

    static ctrlMoveBack() {
        for(let j = Editable.caret-2; j >= 0; j--) { //lowkey idk why i have to subtract 2
            for(const delim of Editable.ctrlDelimiters) {
                if(Editable.edited.value[j] == delim) {
                    //print("delim nigga", j, Editable.edited.value[j], delim);
                    const i = j+1;
                    const count = (Editable.caret-i);
                    //break; //god damn it i swear in js break would work in nested loops
                    //break bruh;
                    return {i, count};
                }
            }
            if(j == 0) {
                const i = 0;
                const count = Editable.caret;
                return {i, count};
            }
        }
        return {i: 0, count: 0}
    }

    static ctrlMoveForward() {
        //for(let j = Editable.caret+1; j < Editable.edited.value.length; j++) { //plus one here to skip the possible delim in front
        //    for(const delim of Editable.ctrlDelimiters) {
        //        if(Editable.edited.value[j] == delim) {
        //            //print("delim nigga", j, Editable.edited.value[j], delim);
        //            const count = j-(Editable.caret+1);
        //            //break; //god damn it i swear in js break would work in nested loops
        //            //break bruh;
        //            return count;
        //        }
        //    }
        //    if(j == Editable.edited.value.length) {
        //        return (Editable.caret+1)-j;
        //    }
        //}
        //return Editable.edited.value.length-(Editable.caret+1);
        for(const delim of Editable.ctrlDelimiters) {
            const index = Editable.edited.value.indexOf(delim, Editable.caret+1); //plus one here so if the caret is on a space we skip it
            if(index != -1) {
                return index-(Editable.caret+1);
            }
        }
        return Editable.edited.value.length-(Editable.caret+1);
    }

    static modify(keycode) { //modify is for delete and page up and what not
        if(keycode == VK_LEFT || keycode == VK_BACK) {
            //Editable.caret = Math.max(0, Editable.caret-1);
            //Editable.caret--;
            //if(Editable.caret < 0) {
            //    Editable.caret = 0;
            //}
            let i = Editable.caret-1;

            if(keycode == VK_BACK) {
                //Editable.edited.value = Editable.edited.value.split("").splice(Editable.caret, 1).join();
                let count = 1;
                if(GetKey(VK_CONTROL)) {
                    const ret = Editable.ctrlMoveBack();
                    i = ret.i;
                    count = ret.count;
                }
                const arr = Editable.edited.value.split("");
                //print("post", arr, i, count);
                arr.splice(i, count);
                Editable.edited.value = arr.join("");
            }else {
                if(GetKey(VK_CONTROL)) {
                    const ret = Editable.ctrlMoveBack();
                    i = ret.i;
                }
            }

            Editable.caret = Math.max(0, i);
        }else if(keycode == VK_RIGHT) {
            //Editable.caret = Math.min(Editable.edited.value.length, Editable.caret+1);
            //Editable.caret++;
            //if(Editable.caret > Editable.edited.value.length) {
            //    Editable.caret = Editable.edited.value.length;
            //}
            
            let i = Editable.caret+1;
            if(GetKey(VK_CONTROL)) {
                i += Editable.ctrlMoveForward();
            }

            Editable.caret = Math.min(Editable.edited.value.length, i);
        }else if(keycode == VK_UP || keycode == VK_PRIOR || keycode == VK_HOME) { //page up
            Editable.caret = Editable.edited.value.length;
        }else if(keycode == VK_DOWN || keycode == VK_NEXT || keycode == VK_END) { //page down
            Editable.caret = 0;
        }else if(keycode == VK_DELETE) {
            //damn i thought you could chain them
            //Editable.edited.value = Editable.edited.value.split("").splice(Editable.caret, 1).join();
            let count = 1;
            if(GetKey(VK_CONTROL)) {
                count += Editable.ctrlMoveForward();
            }
            const arr = Editable.edited.value.split("");
            arr.splice(Editable.caret, count);
            Editable.edited.value = arr.join("");
        }else if(keycode == VK_RETURN && Editable.returntoquit) {
            return Editable.endInput();
        }

        dirty = true;
    }

    static writechar(char) {
        //if(GetKey(VK_CONTROL) && char == "a") { //why is char String.fromCharCode(1) when you hit control a
        //    //lowkey idk about highlighting like that so im just moving the caret
        //    return;
        //}
        //Editable.edited.value = Editable.edited.value.split("").splice(Editable.caret, 0, char).join();
        const arr = Editable.edited.value.split("");
        arr.splice(Editable.caret, 0, char);
        Editable.edited.value = arr.join("");

        Editable.caret += char.length;

        dirty = true;
    }

    //wait writechar literally does this exact thing lemme just modify it
    //static pasteText(text) {
    //    const arr = Editable.edited.value.split("");
    //    arr.splice(Editable.caret, 0, text);
    //    Editable.edited.value = arr.join("");
    //
    //    Editable.caret += text.length;
    //
    //    dirty = true;
    //}

    static endInput(dontcallendinput) {
        print(Editable.edited, Editable.edited.endInput);
        if(!dontcallendinput) {
            Editable.edited.endInput?.();
        }
        Editable.edited = undefined;
        Editable.editing = false;
        print("END INPUT");
        
        dirty = true;
    }
}

class Gradient {
    static LinearGradientBrush(gradientStop, ...args) {
        return new Gradient(gradientStop, d2d.CreateLinearGradientBrush(...args, gradientStop));
    }

    static RadialGradientBrush(gradientStop, ...args) {
        return new Gradient(gradientStop, d2d.CreateRadialGradientBrush(...args, gradientStop));
    }

    constructor(gradientStop, gradient) {
        this.gradientStop = gradientStop;
        this.gradient = gradient;
        //haha lowkey i just remembered that if i actually made an internalPtr property on this object it would be valid to pass this object directly into a d2d function!
        this.internalPtr = this.gradient.internalPtr; //LNMAO
    }

    Release() {
        this.gradient.Release();
        this.gradientStop.Release();
    }
}

class Base { //you know what i was about to say i might get rid of this class and just add it to parent but GenericControl extends it so im keeping an eye on you...
    children = [];
    //children_always_check = [];
    always_check = {}; //stores the INDICES of the children to always check because storing the children in two different arrays was hell

    //visible = true;

    constructor(x, y, width, height, children) {
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
        if(children) {
            for(let i = 0; i < children.length; i++) {
                const child = children[i];
                this.appendChild(child);
                //if(child.alwaysHittest) {
                //    //this.children_always_check.push(child);
                //    this.always_check.push(i);
                //}//else {
                //    this.children.push(child);
                ////}
            }
        }
    }
    
    appendChild(control) {
        if(control.alwaysHittest) {
            this.alwaysHittest = true;
            //this.always_check.push(i+1);
            this.always_check[this.children.length] = true;
            //this.children_always_check.push(control);
        }//else {
            this.children.push(control);
        //}
    }

    removeChild(index) {
        this.children[index].destroy();
        this.children.splice(index, 1);
        delete this.always_check[index];
        //aw man this means i gotta decrement all the boys above this numbe
        const keys = Object.keys(this.always_check);
        for(const key of keys) {
            if(key > index) {
                //this.always_check[key]--;
                this.always_check[key-1] = this.always_check[key];
                delete this.always_check[key];
            }
        }
    }

    mouseDown(mouse) {
        this.children.forEach(c=>c?.mouseDown(mouse));
    }

    mouseUp(mouse) {
        this.children.forEach(c=>c?.mouseUp(mouse));
    }

    //forEach(callback) {
    //    forAllArrays(callback.bind(this), this.children, this.children_always_check);
    //}

    //get length {
    //    return this.children.length+this.children_always_check.length;
    //}

    destroy() {
        //this.addressBox.destroy();
        for(const control of this.children) {
            control.destroy();
        }
        //for(const control of this.children_always_check) {
        //    control.destroy();
        //}
        //this.forEach(control=>control.destroy());
    }
}

class Parent extends Base {
    defaultHT = undefined;

    constructor(x, y, width, height, children, defaultHittestArr) {
        super(x, y, width, height, children);
        this.defaultHT = defaultHittestArr;
    }

    hittest(mouse) {
        //print(`${this.constructor.name} hittest`);
        if(this.alwaysHittest || withinBounds(this, mouse)) {
            //print("hittesting", this.constructor.name);
            /*let ht = this.addressBox.hittest(mouse, this.addressBox.x, this.addressBox.y);
            if(ht) {
                return ht;
            }
            ht = this.windowBox.hittest(mouse, this.windowBox.x, this.windowBox.y);
            if(ht) {
                return ht;
            }
            ht = this.nextPageButton.hittest(mouse, this.nextPageButton.x, this.nextPageButton.y);
            if(ht) {
                return ht;
            }*/
            let ht;//, i; //oh i see the problem here. if every hittest in the first test doesn't work out, i don't loop through the next list because i is already this.children.length. (i need to actually check which ones were checked)

            //for(const control of this.children) {
            for(let i = 0; i < this.children.length; i++) {
                const control = this.children[i];
                const ac = this.always_check.hasOwnProperty(i);
                if(withinBounds(control, mouse)) {
                    if(ac) {
                        this.always_check[i] = false; //since we've already checked this bad boy let's just say it's false
                    }
                    if(ht = control.hittest(mouse)) {//assuming i write the position of each control
                        //return ht; //hell yeah
                        break;
                    }
                }else if(ac) {
                    this.always_check[i] = true;
                }
            }
            //oouuuuhh this new technique has me loop through all of them even if i've already checked em
            // for(const control of this.children_always_check) {
            //const ackeys = Object.keys(this.always_check);
            //print("ackkesy", ackeys, this.constructor.name);
            //for(const index of ackeys) {
            for(const index in this.always_check) {
                if(!this.always_check[index]) {
                    continue;
                }
                const control = this.children[index];
                let subht = control.hittest(mouse);
                if(!ht && subht) {
                    ht = subht;
                    //break;
                }
            }
            if(ht) {
                return ht;
            }
            return this.defaultHT;
        }
    }
}

class GenericControl extends Base {
    constructor(x, y, width, height) {
        super(x, y, width, height, undefined);
    }
    
    resize(width, height) {
        this.width = width;
        this.height = height;
    }
}

class StaticControl extends GenericControl {
    // width = 100;
    // height = 16;
    value = "Sample Text";
    color = [1.0, 1.0, 1.0];

    constructor(x, y, width = 100, height = 16, value = "Sample Text", color = [1.0, 1.0, 1.0]) {
        super(x, y, width, height);
        // this.width = width;
        // this.height = height;
        this.value = d2d.CreateTextLayout(value, font, this.width, this.height);
        this.color = color;
    }

    resize(newwidth, newheight) {
        super.resize(newwidth, newheight);
        this.value.Release();
        this.value = d2d.CreateTextLayout(this.value.text, font, this.width, this.height);
    }

    redraw() {
        colorBrush.SetColor(...this.color);
        d2d.DrawTextLayout(this.x, this.y, this.value, colorBrush);
    }

    hittest(mouse) {

    }

    destroy() {
        this.value.Release();
    }
}

class WatchStaticControl extends StaticControl {
    constructor(x, y, width, height, object, property, color) {
        super(x, y, width, height, object[property], color);
        this.object = object;
        this.property = property;
    }

    redraw() {
        if(this.object[this.property] != this.value.text) {
            this.value.Release();
            this.value = d2d.CreateTextLayout(this.object[this.property], font, this.width, this.height);
        }
        super.redraw();
    }
}

class TextButtonControl extends StaticControl {
    alwaysHittest = true;

    constructor(x, y, width, height, value, textcolor, buttoncolor = [0, 0, 0], callback, parent) {
        super(x, y, width, height, value, textcolor);
        this.buttonColor = buttoncolor;
        this.callback = callback?.bind(parent);
        this.hover = false;
    }

    buttonDown(mouse) {
        this.callback?.(this, mouse);
    }

    redraw() {
        colorBrush.SetColor(...this.buttonColor);
        d2d.FillRectangle(this.x, this.y, this.x+this.width, this.y+this.height, colorBrush);
        if(this.hover) {
            colorBrush.SetColor(1.0, 1.0, 1.0, .4);
            d2d.FillRectangle(this.x, this.y, this.x+this.width, this.y+this.height, colorBrush);
        }
        super.redraw();
    }

    hittest(mouse) {
        this.hover = false;
        if(withinBounds(this, mouse)) {
            this.hover = true;
            return [BUTTON, this];
        }
    }

    destroy() {
        super.destroy();
    }
}

class CheckboxControl extends GenericControl {
    // width = 16;
    // height = 16;
    value = false;
    callback = undefined;
    geometry = undefined;
    mouseover = false;
    alwaysHittest = true;

    constructor(x, y, width = 16, height = 16, value = false, callback, parent) {
        super(x, y, width, height);
        // this.width = width;
        // this.height = height;
        this.value = value;
        this.callback = callback?.bind(parent);

        this.geometry = d2d.CreatePathGeometry();
        const sink = this.geometry.Open();
        sink.BeginFigure(4, 9, D2D1_FIGURE_BEGIN_HOLLOW); //lowkey just looked a picture of a checked boolean parameter in unreal
        sink.AddLine(7, 11);
        sink.AddLine(13, 5);
        sink.EndFigure(D2D1_FIGURE_END_OPEN);
        sink.Close();
        sink.Release();
    }

    buttonDown(mouse) {
        this.value = !this.value;
        this.callback?.(this);
    }

    redraw() {
        colorBrush.SetColor(.3, .3, .3);
        d2d.FillRoundedRectangle(this.x, this.y, this.x+this.width, this.y+this.height, 2, 2, colorBrush);
        colorBrush.SetColor(1.0, 1.0, 1.0);
        if(this.value) {
            //oof
            const prev = d2d.GetTransform(); //i should be able to just do prev.Translation() or something so i can add it so i might have to add that
            const copy = Object.assign({}, prev); //hell yeah brother next best thing to structuredClone
            //print(copy);
            //copy.dx += x; //i mean probably dawg unfortunately matrices are outta my league currently
            //copy.dy += y;
            ////oops wrong properties
            //copy.m[2][0] += x;
            //copy.m[2][1] += y;
            copy._31 += this.x; //ok idk why Matrix3x2F has so many useless properties since this is the only one i had to change
            copy._32 += this.y;
            //print(copy, "POST COPY");
            d2d.SetTransform(copy);
            d2d.DrawGeometry(this.geometry, colorBrush, 2); //no round stroke style for htis one
            d2d.SetTransform(prev);
        }
        d2d.DrawRoundedRectangle(this.x, this.y, this.x+this.width, this.y+this.height, 2, 2, colorBrush, 2, roundStrokeStyle);
    }

    hittest(mouse) {
        this.mouseover = false;
        print("checkbox hittest");
        if(withinBounds(this, mouse)) {
            print("checkboxm ouseofver");
            this.mouseover = true;
            return [BUTTON, this];
        }
    }

    destroy() {
        this.geometry.Release();
    }
}

class DropdownControlMenu { //a singleton for controls that need a dropdown (dropdown as in the way i use it in shadereditor2.js)
    //the reason i need a whole new class/pane for this is because if i drew the entire dropdown in an individual dropdown control (like i was doing in shadereditor2.js) and i had another control **right below** the dropdown then the hittest would (probably) go off of (oh fuck! right as i was writing this part i just realized that my assumptions were wrong and techincally i didn't have to make this class (im gonna keep it though because it'll probably help me))
    x = 0;
    y = 0;
    width = 0;
    height = 0;
    rowHeight = 16;
    options = [];
    sticky = true; //OHHHH ("it's getting sticky in this bitch")
    static instance = undefined;

    static open(x, y, options, callback, width = 0) {
        if(this.instance) {
            this.close();
        }
        //this.instance.x = x; //this as in DropdownControlMenu (static)
        //this.instance.y = y;
        //this.instance.options = options;
        if(!width) {
            //we calculating it manually
            const longeststr = options.toSorted((a, b) => b.length-a.length)[0];
            const temp = d2d.CreateTextLayout(longeststr, font, w, h);
            width = temp.GetMetrics().width+4; //i might have to make a d2d.GetMetricsForText(text, font, maxwidth, maxheight)
            temp.Release();
        }
        if(x+width > w) {
            x -= (x+width)-w;
        }
        //this.instance.width = width;
        this.instance = new DropdownControlMenu(x, y, options, callback, width);
        this.instance.height = options.length*this.instance.rowHeight; //just in case i decide to change rowHeight
        //this.instance.width = ;//calc is short for calculator im just using slang
        panes.push(this.instance);
        //this.instance.height = 16; //propbbaly
    }

    constructor(x, y, options, callback, width) {
        this.x = x;
        this.y = y;
        for(const option of options) {
            this.options.push(d2d.CreateTextLayout(option, font, w, h)); //this.options is by default an empty array []
        }
        //this.options = options;
        this.width = width;
        //this.height = height;
        this.callback = callback;
    }

    redraw() {
        //const dropdownheight = control.height/(control.args.length+1); //lol
        // /mangomangomango/gymuisd
        colorBrush.SetColor(0xffffff); //nice :)
        d2d.FillRectangle(0, 0, this.width, this.height, colorBrush);
        for(let i = 0; i < this.options.length; i++) {
            const y = i*this.rowHeight;
            colorBrush.SetColor(0.0, 0.0, 0.0);
            d2d.DrawRectangle(0, y, this.width, y+this.rowHeight, colorBrush, 1, roundStrokeStyle);
            //colorBrush.SetColor(1.0, 1.0, 1.0);
            //d2d.FillRectangle(control.x+textwidth, control.y+(i+1)*dropdownheight, control.x+control.width, (control.y+(i+1)*dropdownheight)+dropdownheight, colorBrush);
            //colorBrush.SetColor(0.0, 0.0, 0.0);
            d2d.DrawTextLayout(0, y, this.options[i], colorBrush);
            //d2d.DrawText(this.options[i], font, 0, y, w, h, colorBrush);
        }
    }

    hittest(mouse) {
        for(let i = 0; i < this.options.length; i++) {
            if(withinBounds({x: 0, y: i*this.rowHeight, width: this.width, height: this.rowHeight}, mouse)) {
                const tempobjectthatmakesmecringe = { //this is weird but valid i guess lol
                    buttonDown: function(mouse) {
                        //using DropdownControlMenu.instance because i can't bothered to bind(this)
                        DropdownControlMenu.instance.callback(DropdownControlMenu.instance.options[i].text); //.text because they are now TextLayouts
                        DropdownControlMenu.close();
                    },
                };
                return [BUTTON, tempobjectthatmakesmecringe]; //should probably make a callback version but i did it like this because yk
            }
        }
        return [NULL];
    }

    static close() {
        //print("Close DropdownControlMenu");
        this.instance.destroy();
        this.instance = undefined;
    }

    destroy() {
        //idk yet hold on.
        for(const option of this.options) {
            option.Release(); //oops forgot that
        }
        panes.splice(panes.indexOf(this), 1);
    }
}

class EditDropdownControl extends GenericControl {
    options = [];
    value = undefined;
    // width = 0;
    // height = 0;

    constructor(x, y, width, height, options, value, submitCallback, delayedOptionsCallback, parent) {
        super(x, y, width, height);
        // this.width = width;
        // this.height = height;
        this.options = options;
        this.delayedOptionsCallback = delayedOptionsCallback?.bind(parent);
        this.value = value ?? options?.[0] ?? this.delayedOptionsCallback()[0]; //lmao
        this.submitCallback = submitCallback?.bind(parent);

        this.layout = d2d.CreateTextLayout(this.value, font, width, height); //OOHHHH
    }

    endInput() {
        print("EditDropdownControl endInput");
        DropdownControlMenu.close();
        this.value = this.options.toSorted((a,b)=>a.localeCompare(b)).toSorted(diyfuzzysort(this.value))[0];
        this.submitCallback?.(this);
    }

    onEdit(newvalue) {
        print("EditDropdownControl onEdit");
        Editable.endInput(true);
        //print("binf this idk?", this.options); //apparently i didn't have to bind rthis (ok no i lied without bind(this), this function is bound to DropdownControlMenu!)
        //print(this instanceof DropdownButtonControl, newvalue);
        this.value = newvalue;
        this.submitCallback?.(this);
    }

    buttonDown(mouse) {
        //i wasn't originally planning on using mouse.x and mouse.y but it works good enough lmao
        //oh shoot mouse isn't screenspace lemme fix that
        Editable.beginInput(0, this);
        if(this.delayedOptionsCallback) {
            this.options = this.delayedOptionsCallback();
        }
        DropdownControlMenu.open(mouse.x+camera.x, mouse.y+camera.y+this.height, this.options, this.onEdit.bind(this));
    }
    
    updateText() {
        //print(this.layout.text, this.value); //wait what the fuck why does this.layout.text give me the actual boolean value instead of a string (yeah oops i was trolling on the v8 side)
        //if(this.layout.text !== this.value) { //OH MY GOD THIS IS THE FIRST TIME I'VE EVER NEEDED TO USE A STRICT OPERATOR BECAUSE I NEED TO TELL THE DIFFERENCE BETWEEN false AND 0!!!!!!
        if(this.layout.text !== this.value) { //NEVERMIND I STILL HAVE TO USE IT ANYWAYS BECAUSE THE STRING "0x14" IS BEING COERCED INTO A NUMBER (20) SO THE COMPARISON "0x14" != 20 IS FALSE!
            this.layout.Release();
            this.layout = d2d.CreateTextLayout(this.value, font, this.width, this.height); //100, 16);
            if(this.autoresize) {
                const {widthIncludingTrailingWhitespace} = this.layout.GetMetrics();
                this.width = Math.max(24, 16+widthIncludingTrailingWhitespace);
            }
        }
    }
    
    redraw() {
        //const halfwidth = PropertyMenu.instance.width/2;
        colorBrush.SetColor(0xcccccc);
        d2d.FillRoundedRectangle(this.x, this.y, this.x+this.width-4, this.y+this.height, 2, 2, colorBrush);
        colorBrush.SetColor(229/255, 229/255, 229/255);
        d2d.DrawRoundedRectangle(this.x, this.y, this.x+this.width-4, this.y+this.height, 2, 2, colorBrush, 2, roundStrokeStyle); //gotta subtract 4 because of the 2 strokewidth

        this.updateText();

        colorBrush.SetColor(0.0, 0.0, 0.0);
        
        if(Editable.edited == this) {
            //const t = d2d.CreateTextLayout(this.value.slice(0, Editable.caret), font, 100, 16);
            //const {widthIncludingTrailingWhitespace} = t.GetMetrics(); //if there are trailing spaces it doesn't show that and it's weird

            
            const hit = this.layout.HitTestTextPosition(Editable.caret, false); //who knew this function's main purpose was this use case
            //print(hit.x, hit.hitTestMetrics.width);
            const caretX = hit.x+this.x; //plus 4 because im drawing the text 4 pixels away from 0
            const caretY = hit.y+this.y;

            
            //const pos = ; //lowkey might have to make a text layout to see how wide each character is
            d2d.DrawLine(caretX, caretY, caretX, caretY+16, colorBrush, 1, roundStrokeStyle);
            //t.Release();
        }
        d2d.DrawTextLayout(this.x, this.y, this.layout, colorBrush);    
    }

    hittest(mouse) {
        if(withinBounds(this, mouse)) {
            return [BUTTON, this]; //well since i want the button functionality AND the edit control part im just gonna emulate what TEXT does in the buttonDown callback
        }
    }

    destroy() {
        
    }
}

class DropdownButtonControl extends GenericControl {
    options = [];
    callback = undefined; 
    value = undefined;
    // width = 0;
    // height = 0;

    constructor(x, y, width, height, options, value, callback) {
        super(x, y, width, height);
        this.options = options;
        this.callback = callback;
        this.value = value ?? options[0];
        // this.width = width;
        // this.height = height;
        print("lazymaxxing by not creating a text layout for value in dropdown button ctonrol");
    }

    onEdit(newvalue) {
        //print("binf this idk?", this.options); //apparently i didn't have to bind rthis (ok no i lied without bind(this), this function is bound to DropdownControlMenu!)
        //print(this instanceof DropdownButtonControl, newvalue);
        this.value = newvalue;
        this.callback?.(newvalue);
    }

    buttonDown(mouse) {
        //i wasn't originally planning on using mouse.x and mouse.y but it works good enough lmao
        //oh shoot mouse isn't screenspace lemme fix that
        DropdownControlMenu.open(mouse.x+camera.x, mouse.y+camera.y, this.options, this.onEdit.bind(this));
    }
    
    redraw() {
        colorBrush.SetColor(0.0, 0.0, 0.0);
        d2d.DrawRoundedRectangle(this.x, this.y, this.x+this.width-4, this.y+this.height, 1, 1, colorBrush, 2, roundStrokeStyle);
        colorBrush.SetColor(229/255, 229/255, 229/255);
        d2d.FillRoundedRectangle(this.x, this.y, this.x+this.width-4, this.y+this.height, 1, 1, colorBrush);
        colorBrush.SetColor(0.0, 0.0, 0.0);
        d2d.DrawText(this.value, font, this.x, this.y, w, h, colorBrush); //lazy...
    }

    hittest(mouse) {
        if(withinBounds(this, mouse)) {
            return [BUTTON, this];
        }
    }

    destroy() {
        
    }
}

class EditControl extends GenericControl {
    // width = 24;
    // height = 16;
    value = "";
    data = undefined;
    layout = undefined;
    autoresize = false;

    verifyInput() {
        if(this.value == "NULL" || this.value == "null" || this.value == "undefined"){
            this.value = undefined;
        }else if(this.data == "number") {
            this.value = Number(this.value) || 0; //NaN is falsy but not nullish (no nullish coalescing today)
        }else if(this.data == "float") {
            this.value = parseFloat(this.value) || 0.0;
        }else if(this.data == "bigint") {
            this.value = BigInt(this.value) || 0n;
        }
        this.updateText();
    }

    endInput() {
        print("huzz tuzz");
        this.verifyInput();
        this.onEndInput?.(this);
    }

    constructor(x, y, width = 24, height = 16, data, value, endInput, parent) {
        super(x, y, width, height);
        this.layout = d2d.CreateTextLayout(value, font, width, height); //OOHHHH
        //this.endInput = endInput?.bind(this);// ?? this.verifyInput;
        /*if(endInput) {
            if(bind) {
                this.onEndInput = endInput.bind(this);
            }else {
                this.onEndInput = endInput;
            }
        }*/
        this.onEndInput = endInput?.bind(parent);
        // this.width = width;
        // this.height = height;
        this.data = data;
        //if(!value) {
        //    if(data) {
        //        this.verifyInput();
        //    }else {
        //        this.value = "";
        //    }
        //}else {
        //    this.value = value; //CHANGING VALUE DOWN HERE CAUSES REDRAW TO UPDATE THE SIZE SINCE IT'S DIFFERENT!
        //}
        this.value = value ?? "";
        this.verifyInput();
    }

    resize(newwidth, newheight) {
        super.resize(newwidth, newheight);
        this.layout.Release();
        this.layout = d2d.CreateTextLayout(this.layout.text, font, this.width, this.height); //OOHHHH
    }

    updateText() {
        //print(this.layout.text, this.value); //wait what the fuck why does this.layout.text give me the actual boolean value instead of a string (yeah oops i was trolling on the v8 side)
        //if(this.layout.text !== this.value) { //OH MY GOD THIS IS THE FIRST TIME I'VE EVER NEEDED TO USE A STRICT OPERATOR BECAUSE I NEED TO TELL THE DIFFERENCE BETWEEN false AND 0!!!!!!
        if(this.layout.text !== this.value) { //NEVERMIND I STILL HAVE TO USE IT ANYWAYS BECAUSE THE STRING "0x14" IS BEING COERCED INTO A NUMBER (20) SO THE COMPARISON "0x14" != 20 IS FALSE!
            this.layout.Release();
            this.layout = d2d.CreateTextLayout(this.value, font, this.width, this.height); //100, 16);
            if(this.autoresize) {
                const {widthIncludingTrailingWhitespace} = this.layout.GetMetrics();
                this.width = Math.max(24, 16+widthIncludingTrailingWhitespace);
            }
        }
    }
    
    redraw() {
        //const halfwidth = PropertyMenu.instance.width/2;
        colorBrush.SetColor(0xcccccc);
        d2d.FillRoundedRectangle(this.x, this.y, this.x+this.width-4, this.y+this.height, 2, 2, colorBrush);
        colorBrush.SetColor(229/255, 229/255, 229/255);
        d2d.DrawRoundedRectangle(this.x, this.y, this.x+this.width-4, this.y+this.height, 2, 2, colorBrush, 2, roundStrokeStyle); //gotta subtract 4 because of the 2 strokewidth

        this.updateText();

        colorBrush.SetColor(0.0, 0.0, 0.0);
        
        if(Editable.edited == this) {
            //const t = d2d.CreateTextLayout(this.value.slice(0, Editable.caret), font, 100, 16);
            //const {widthIncludingTrailingWhitespace} = t.GetMetrics(); //if there are trailing spaces it doesn't show that and it's weird

            
            const hit = this.layout.HitTestTextPosition(Editable.caret, false); //who knew this function's main purpose was this use case
            //print(hit.x, hit.hitTestMetrics.width);
            const caretX = hit.x+this.x; //plus 4 because im drawing the text 4 pixels away from 0
            const caretY = hit.y+this.y;

            
            //const pos = ; //lowkey might have to make a text layout to see how wide each character is
            d2d.DrawLine(caretX, caretY, caretX, caretY+16, colorBrush, 1, roundStrokeStyle);
            //t.Release();
        }
        d2d.DrawTextLayout(this.x, this.y, this.layout, colorBrush);    
    }

    hittest(mouse) {
        //const halfwidth = PropertyMenu.instance.width/2;
        if(withinBounds(this, mouse)) {
            const i = this.layout.HitTestPoint(mouse.x-this.x, mouse.y).hitTestMetrics.textPosition;
            return [TEXT, [i, this]];
        }
        //no return NULL here because im doing it specially (which is weird)
        //(i mean that i use return [NULL] at the end of PropertyMenu's hittest if nothing was hit)
    }

    destroy() {
        //this.name.Release();
        this.layout.Release();
    }
}

class DropdownList { //damn this instantly gets rid of the optimizations i was trying to make with VerticalScrollBox...
    constructor(x, y, width, elements, title, color = [1.0, 1.0, 1.0]) {
        this.x = x;
        this.y = y;
        this._width = width; //ok this is weird lowkey
        //this._height = DropdownList.closedHeight;
        this.open = false;
        this.elements = elements;
        this.title = d2d.CreateTextLayout("> "+title, font, w, h);
        this.color = color;
    }

    static closedHeight = 12;

    get width() {
        if(this.open) {
            return this._width;
        }else {
            return this.title.GetMetrics().width;
        }
    }

    get height() {
        let _height = DropdownList.closedHeight;
        if(this.open) {
            let cumHeight = 0;
            for(const {height} of this.elements) {
                cumHeight += height;
            }
            _height += cumHeight; //this._height;
        }
        return _height;
    }

    redraw() {
        colorBrush.SetColor(...this.color);
        d2d.DrawTextLayout(this.x, this.y, this.title, colorBrush);

        if(this.open) {
            let cumHeight = 0;
            for(const element of this.elements) {
                element.redraw(this.x+14, this.y+DropdownList.closedHeight+cumHeight);
                cumHeight += element.height;
            }
        }
    }

    buttonDown() {
        this.open = !this.open;
    }

    hittest(mouse) {
        if(withinBounds({x: this.x, y: this.y, width: this.width, height: DropdownList.closedHeight}, mouse)) {
            return [BUTTON, this];
        }else if(this.open) {
            let cumHeight = 0;
            for(const element of this.elements) {
                const ht = element.hittest(mouse, this.x+14, this.y+DropdownList.closedHeight+cumHeight);
                if(ht) {
                    return ht;
                }
                cumHeight += element.height;
            }
        }
    }

    destroy() {
        this.title.Release();
        for(const element of this.elements) {
            element.destroy();
        }
    }
}

class VerticalScrollBox extends GenericControl {
    //x = 0;
    //y = 0;
    //width = 0;
    //height = 0;

    scroll = {
        y: 0,
        velocity: 0,
        barHeight: 20,
    };

    constructor(x, y, width, height, elements, smooth = true) { //assuming we know the position when we create it (unlike CheckboxControl and the other control ones like that yk)
        super(x, y, width, height);
        //this.x = x;
        //this.y = y;
        //this.width = width;
        //this.height = height;
        this.elements = elements;
        this.smooth = smooth;
        this.rectangle = d2d.CreateRectangleGeometry(this.x, this.y, this.x+this.width, this.y+this.height);
    }

    getElementsHeight(elementtofind = undefined) {
        let height = 0;
        for(const element of this.elements) {
            if(element == elementtofind) {
                break;
            }
            height += element.height;
        }
        return height;
    }

    getFirstVisibleElementIndexAndCountUntilOffscreen() {
        let fve = undefined;
        let index = 0;
        let elementHeights = 0;
        let count = 0;
        let firstVisibleHeight = 0;
        //for(const element of this.elements) {
        for(let i = 0; i < this.elements.length; i++) {
            const element = this.elements[i];
            if(this.scroll.y <= elementHeights) { //haha i have to use less than or EQUAL because not using smooth scrolling would snap to each height
                if(!fve) {
                    //actually i'd want the element BEFORE this one because if it's really long it would get cut off otherwise
                    if(i == 0) {
                        index = i;
                        fve = element;
                        firstVisibleHeight = elementHeights;
                    }else {
                        index = i-1;
                        fve = this.elements[i-1];
                        firstVisibleHeight = elementHeights-fve.height;
                        count++;
                    }
                }else {
                    count++;
                    //if(this.scroll.y+this.height <= elementHeights) {
                    if(this.scroll.y+this.height < elementHeights) {
                        //count++; //there we go
                        break;
                    }
                }
                //return [fve, i];
            }else if(i == this.elements.length-1 && !fve) {
                index = i;
                fve = element;
                firstVisibleHeight = elementHeights;
            }
            elementHeights += element.height;
        }
        count++;
        return [index, firstVisibleHeight, count];
    }

    redraw() {
        //not gonna draw a background tho
        if(Math.abs(this.scroll.velocity) > 0.01) {
            this.scroll.y = Math.min(this.getElementsHeight(), Math.max(0, this.scroll.y+this.scroll.velocity));
            if(this.smooth) {
                this.scroll.velocity *= .95;
            }else {
                this.scroll.velocity = 0;
            }
            //dirty = true; //lol! (aw damn wait it's already true (OHHHHHhhhh so that's what i meant (yeah when redraw is called dirty is already true and after it's called i set dirty to false...)))
        }

        
        d2d.PushLayer(new D2D1.LayerParameters(D2D1.InfiniteRect(), this.rectangle)); //clips any overflowing text for meh (count has 1 extra that is usually outside the border)
        
        //const arrlen = (this.height/this.getElementsHeight())+1; //yeah no
        const [firstVisibleElementIndex, firstVisibleHeight, count] = this.getFirstVisibleElementIndexAndCountUntilOffscreen();

        //print("fvei", firstVisibleElementIndex, firstVisibleHeight, count);
        print("amount to draw", count);

        //for(let i = 0; i < arrlen; i++) {
        let cumHeight = 0; //cumulative
        const distancebecausescrolled = firstVisibleHeight-this.scroll.y;
        //print("distance becase scro", distancebecausescrolled);
        for(let i = 0; i < count; i++) {
            const element = this.elements[firstVisibleElementIndex+i];
            element.redraw(this.x, this.y+cumHeight+distancebecausescrolled);
            cumHeight += element.height;
            //const j = Math.floor(this.scroll.y) + i;
            //const text = this.contents[j];
            //if(text) d2d.DrawTextLayout(0, (i-(this.scroll.y%1))*textheight, text, colorBrush);
        }
        d2d.PopLayer();

        const scrollbarY = (this.scroll.y/this.getElementsHeight())*(this.height-this.scroll.barHeight); //-barHeight because that's how tall i've decided to make the scrollbar lol

        d2d.FillRectangle(this.x+this.width-12, this.y+scrollbarY, this.x+this.width-2, this.y+scrollbarY+this.scroll.barHeight, colorBrush);
    }

    //lowkey idk about the scrollbar being draggable i might have to think on that

    //preDrag(mouse, data) { //predrag does not send mouse in client space or whatever the word to describve it its 
    //    //const clienty = (this.y-this.scroll.y)-mouse.y;
    //    //print(clienty, "clienty", this.scroll.y);
    //    const elementsHeight = this.getElementsHeight();
    //    const scrollbarY = (this.scroll.y/elementsHeight)*(this.height-this.scroll.barHeight);
    //    const tempscrollobjecttohelpmedothis = {x: 0, y: scrollbarY, onDrag: (function() { //oh i was supposed to use scrollBarY instead of this.scroll.y
    //        print(tempscrollobjecttohelpmedothis.y);
    //        this.scroll.y = clamp(0, elementsHeight, (tempscrollobjecttohelpmedothis.y/(this.height-this.scroll.barHeight))*elementsHeight); //! (subtracting height - barHeight so that the mouse doesn't slip away when you drag to the bottom!)
    //        //ok wait this kinda sucks idk imma do it right in jbsblueprints tho trust.
    //    }).bind(this)};
    //    return tempscrollobjecttohelpmedothis;
    //}

    hittest(mouse) {
        const scrollbarY = (this.scroll.y/this.getElementsHeight())*(this.height-this.scroll.barHeight);
        if(withinBounds({x: this.width-12, y: scrollbarY, width: 10, height: this.scroll.barHeight}, mouse)) {
            //return [DRAG]; //no data because im doing it kinda weird
        }else {
            const [firstVisibleElementIndex, firstVisibleHeight, count] = this.getFirstVisibleElementIndexAndCountUntilOffscreen();

            let cumHeight = 0; //cumulative
            const distancebecausescrolled = firstVisibleHeight-this.scroll.y;
            for(let i = 0; i < count; i++) {
                const element = this.elements[firstVisibleElementIndex+i];
                //if(withinBounds({x: 0, y: cumHeight+distancebecausescrolled, width: element.width, height: element.height}, mouse)) {
                
                const ht = element.hittest(mouse, 0, cumHeight+distancebecausescrolled);
                if(ht) {
                    return ht;
                }
                //}
                //element.redraw(0, cumHeight+distancebecausescrolled);
                cumHeight += element.height;
            }    
        }
        
        return [WHEEL, this]; //oops it wanted to scroll on BlueprintMenu since it was the currently hit **pane**
    }

    wheel(wp) {
        //print(wp);
        const scrollY = -wp/120
        print("scrollY:",scrollY);
        if(this.smooth) {
            print("smoove scrole");
            //oh i was wondering why this new version scrolled slower and it was because i changed how it worked so i gotta add more to the velocity now
            //this.scroll.velocity += scrollY/5;
            this.scroll.velocity += scrollY*2.5;
        }else {
            const [firstVisibleElementIndex] = this.getFirstVisibleElementIndexAndCountUntilOffscreen();
            this.scroll.velocity += this.elements[firstVisibleElementIndex].height*scrollY; //*scrollY for negative (wait why did i add the velo,city?)
        }
    }

    clear() {
        for(const element of this.elements) {
            element.destroy();
        }
        this.elements = [];
    }

    destroy() {
        this.clear();
        this.rectangle.Release(); //almost forgot
    }
}

class HorizontalListBox extends Parent {
    //children = [];
    constructor(x, y, width, height, children) {
        super(x, y, width, height, children, undefined);
        // this.x = x;
        // this.y = y;
        // this.width = width;
        // this.height = height;
        // this.children = children;
    }

    appendChild(child) {
        //this.children.push(child);
        super.appendChild(child);
        this.resize(this.width, this.height); //lol
    }

    removeChild(index) {
        //this.children[index].destroy();
        //this.children.splice(index, 1);
        super.removeChild(index);
        this.resize(this.width, this.height);
    }

    resize(newwidth, newheight) {
        //super.resize(newwidth, newheight);
        this.width = newwidth;
        this.height = newheight;
        const ratio = this.width/this.children.length;
        let x = this.x;
        for(const child of this.children) {
        //this.forEach(child => {
            child.x = x;
            child.y = this.y;
            print(child.x, child.y, child.width, child.height, ratio, this.height);
            child.resize(ratio, this.height);
            print(child.width, child.height);
            //child.width = ratio;
            //child.height = this.height;
            x += ratio;
        //});
        }
    }

    redraw() {
        for(const child of this.children) {
            child.redraw(); //this.x+child.x, this.y+child.y);
        }
        //forAllArrays((function(child) {
        //    child.redraw();//this.x+child.x, this.y+child.y);
        //})/*.bind(this)*/, this.children, this.children_always_check);
        //this.forEach(child => child.redraw());
    }

    //hittest(mouse, x, y) {
    //    if(withinBounds({x, y, width: this.width, height: this.height}, mouse)) {
    //        let ht;
    //        for(const child of this.children) {
    //            if(ht = child.hittest(mouse, x+child.x, y+child.y)) {
    //                return ht;
    //            }
    //        }
    //    }
    //}

    //destroy() {
    //    for(const child of this.children) {
    //        child.destroy();
    //    }
    //}
}

class ImageButtonControl extends GenericControl {
    defaultimg = undefined;
    mousedownimg = undefined;
    mouseover = false;
    down = false;
    alwaysHittest = true;
    
    //looks for path/imagename.png
    //looks for path/imagename_mousedown.png
    //assuming png for imagename
    constructor(x, y, width, height, path, imagename, callback, wic) {
        super(x, y, width, height);
        if(!wic) {
            this.wic = InitializeWIC();
            ScopeGUIDs(this.wic);
        }else {
            this.wic = wic;
        }

        
        this.defaultimg = d2d.CreateBitmapFromWicBitmap(this.wic.LoadBitmapFromFilename(`${path}/${imagename}.png`, this.wic.GUID_WICPixelFormat32bppPBGRA), true);
        print(`${path}\\${imagename}.png`);
        this.mousedownimg = d2d.CreateBitmapFromWicBitmap(this.wic.LoadBitmapFromFilename(`${path}/${imagename}_mousedown.png`, this.wic.GUID_WICPixelFormat32bppPBGRA), true);
        print(`${path}\\${imagename}_mousedown.png`);
        this.mousehover = d2d.CreateBitmapFromWicBitmap(this.wic.LoadBitmapFromFilename(`${path}/${imagename}_hover.png`, this.wic.GUID_WICPixelFormat32bppPBGRA), true);
        print(`${path}\\${imagename}_hover.png`);

        //print(this.defaultimg.GetSize(), this.defaultimg.GetPixelSize());

        if(!width) {
            this.width = this.defaultimg.GetSize().width;
        }
        if(!height) {
            this.height = this.defaultimg.GetSize().height;
        }

        // this.width = width;
        // this.height = height;
        
        if(!wic) { //checking again if wic wasn't passed
            this.wic.Release();
        }

        this.callback = callback;
    }

    buttonDown() {
        this.down = true;
        this.callback();
    }

    buttonUp() {
        this.down = false;
    }    

    redraw() {
        let texture = this.defaultimg;
        if(this.down) {
            texture = this.mousedownimg;
        }else if(this.mouseover) {
            texture = this.mousehoverimg;
        }
        d2d.DrawBitmap(texture, this.x, this.y, this.x+this.width, this.y+this.height);
    }

    hittest(mouse) {
        this.mouseover = withinBounds(this, mouse);
        return [BUTTON, this];
    }

    destroy() {
        this.defaultimg.Release();
        this.mousedownimg.Release();
    }
}

var panes = [];
var hit = {};
globalThis.dirty = false;
var camera = {x: 0, y: 0};
var throwawayObjects = [];
var draws = 0;
var activeButton = undefined;
//var d2d, colorBrush, font;

class D2DWindow {
    static singleton = undefined;

    constructor(exStyle, winclass, title, style, x, y, cx, cy, drawBackground) {
        if(D2DWindow.singleton) {
            printNoHighlight("yo you CANNOT make 2 D2DWindows and things will probably go wrong past this point...\x07");
        }
        D2DWindow.singleton = this; //wait can you do this? (yes surprisingly)
        //this.panes = [];
        //this.hit = {};
        //this.dirty = false;
        //this.camera = {x: 0, y: 0};
        this.winclass = winclass;
        this.drawBackground = drawBackground;
        this.realwindowproc = winclass.windowProc;
        this.width = cx;
        this.height = cy;
        //this.throwawayObjects = [];
        winclass.windowProc = this.windowProc.bind(this);
        CreateWindow(exStyle, winclass, title, style, x, y, cx, cy, NULL, NULL, hInstance);
    }

    init(hwnd) {
        const initobj = {};
        initobj.d2d = createCanvas("d2d", ID2D1DeviceContext, hwnd);
        initobj.colorBrush = initobj.d2d.CreateSolidColorBrush(1.0, 1.0, 1.0, 1.0);
        initobj.font = initobj.d2d.CreateFont(NULL, 12);
        initobj.roundStrokeStyle = initobj.d2d.CreateStrokeStyle(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND, NULL, D2D1_DASH_STYLE_SOLID, 0, D2D1_STROKE_TRANSFORM_TYPE_NORMAL);
        return initobj;
    }

    paint() {
        d2d.BeginDraw();

        if(this.drawBackground) {
            this.drawBackground();
        }else {
            const RGB_COLOR = GetSysColor(this.winclass.hbrBackground);
            const [r,g,b] = [GetRValue(RGB_COLOR), GetGValue(RGB_COLOR), GetBValue(RGB_COLOR)];
            colorBrush.SetColor(r/255, g/255, b/255);
            d2d.FillRectangle(0, 0, this.width, this.height, colorBrush);
        }

        for(const pane of panes) {
            d2d.SaveDrawingState();
            //oh wait you can't do this d2d.GetTransform().Translation as it returns the straight up D2D1_MATRIX_3X2_F
            let x = pane.x;
            let y = pane.y;
            if(!pane.sticky) {
                x+=camera.x;
                y+=camera.y;
            }
            d2d.SetTransform(Matrix3x2F.Translation(x, y));
            //defaultBGG.SetTransform(Matrix3x2F.Multiply(Matrix3x2F.Translation(pane.x, pane.y), Matrix3x2F.Scale(pane.width/100, pane.height/100, pane.x, pane.y)));
            //defaultBGG.SetTransform(Matrix3x2F.Scale(pane.width, pane.height, 0, 0));
            //d2d.FillRectangle(0, 0, pane.width, pane.height, defaultBGG);
            pane.redraw();
            d2d.RestoreDrawingState();
        }
    
        //defaultBGG.SetTransform(Matrix3x2F.Identity());
        d2d.EndDraw();
    
        for(const obj of throwawayObjects) {
            //print(obj.Release(), "throwaway release");
            if((r = obj.Release()) != 0) { //this crazy line bruh boy slow down
                print(r, "throwaway release still has references?!!!");
            }
        }
        throwawayObjects = []; //SHOULD be garbage collected    
    }

    windowProc(hwnd, msg, wp, lp) {
        //print(Int_To_WM(msg), Math.random());
        let returnvalue = this.realwindowproc(hwnd, msg, wp, lp);

        if(msg == WM_CREATE) {
            this.paint();
            //SetTimer(hwnd, 0, 16); //lowkey this is only for BlueprintMenu smooth scrolling lmao
        }else if(msg == WM_PAINT) {
            dirty = true;
            //const ps = BeginPaint(hwnd);
            //StretchDIBits(ps.hdc, 0, 0, w, h, 0, 0, w, h, drawing, w, h, 32, BI_RGB, SRCCOPY); //using stretchdibits because i didn't add SetDIBitsToDevice lol
            //EndPaint(hwnd, ps);
        }else if(msg == WM_SIZE) {
            let oldw = this.width;
            let oldh = this.height;
            let wid = LOWORD(lp);
            let hei = HIWORD(lp);
            this.width = wid;
            this.height = hei;
            d2d.Resize(wid, hei); //hell yeah
            for(const pane of panes) {
                pane.windowResized?.(oldw, oldh);
            }
            dirty = true;
        }/*else if(msg == WM_MOUSEMOVE) { //oops i thought mousemove was the best place to check and change the cursor BUT WM_SETCURSOR is actually made for this lmao
            //using SetCursor in here would cause flickering!
        }*/else if(msg == WM_SETCURSOR) { //https://stackoverflow.com/questions/19257237/reset-cursor-in-wm-setcursor-handler-properly
            //print("setcursornigga");
            const mouse = GetCursorPos();//{x: GET_X_LPARAM(lp), y: GET_Y_LPARAM(lp)}; //i was originally using WM_MOUSEMOVE which passed the mouse position in the lp
            ScreenToClient(hwnd, mouse); //fun (returns nothing and modifies the object)
            //mouse.x -= camera.x;
            //mouse.y -= camera.y;
            //let result = 0;
            hit = {};
            //hitpane = undefined;
            //for(const pane of panes) {
            //    const mca = {x: mouse.x, y: mouse.y}; //mouse camera adjusted
            //    if(!pane.sticky) {
            //        mca.x -= camera.x;
            //        mca.y -= camera.y;
            //    }
            //    if(withinBounds(pane, mca)) {
            //        //hitpane = undefined;
//
            //        const clientmouse = {x: mca.x-pane.x, y: mca.y-pane.y};
            //        //mouse.x -= pane.x;
            //        //mouse.y -= pane.y; //to client (oops i was directly modifying mouse)
            //        const [legit, data] = pane.hittest(clientmouse);
            //        //hit |= legit; //lowkey i was just OR ing them together to make it easier
            //        //if(legit != 0) {
            //            hit.result = legit;
            //            hit.pane = pane;
            //            if(legit) {
            //                //hitpane = legit == BUTTON ? data : pane;
            //                hit.data = data;
            //            }
            //        //}
            //    }
            //}
            for(let i = panes.length-1; i >= 0; i--) {
                const pane = panes[i];
                const mca = {x: mouse.x, y: mouse.y}; //mouse camera adjusted
                if(!pane.sticky) {
                    mca.x -= camera.x;
                    mca.y -= camera.y;
                }

                const clientmouse = {x: mca.x-pane.x, y: mca.y-pane.y};

                const ht = pane.hittest(clientmouse);

                if(ht) {
                    const [legit, data] = ht;

                    hit.result = legit;
                    hit.pane = pane;
                    if(legit) {
                        hit.data = data;
                    }
                    break;
                }
            }
            //wait why is this loop not in the next if statement
            for(const [flag, cursor] of hittestarr) {
                //if((hit & flag) == flag) {
                if(hit.result == flag) { //i had to change hit to be only one value because if the topmost pane set TOP and a pane below set MOVE, it would use MOVE because it was the last element of hittestarr (wait a second that's not even the current problem)
                    SetCursor(cursor);
                }
            }
            if(hit.result) {
                dirty = true;
                //SetCursor(arrow);
                //return 1; //return true from WM_SETCURSOR to halt further processing (AND MOST IMPORTANTLY, RETURNING A VALUE OTHER THAN 0 DOES NOT CALL DefWindowProc NO MATTWR WHAT!!!!!)
                returnvalue = 1; //not totally sure why i actually returned right here because another event is required to actually redraw the shit since i set it to be dirty
            }
        }else if(msg == WM_LBUTTONDOWN) {
            SetCapture(hwnd);
            const mouse = {x: GET_X_LPARAM(lp)-camera.x, y: GET_Y_LPARAM(lp)-camera.y}; //lp is client mouse pos
            
            if(hit.result) {
                //if((hit.result & BUTTON) == BUTTON) {
                if(hit.result == BUTTON) {
                    hit.data.buttonDown(mouse);
                    activeButton = hit.data;
                }else //if((hit.result & MOVE) == MOVE) {
                if(hit.result == MOVE) {
                    Draggable.select(hit.pane, mouse, false, false);
                }else if(hit.result == CONTEXTMENU || hit.result == WHEEL) {
                    //no
                }else if(hit.result == DRAG) {
                    Draggable.select(hit.pane.preDrag?.(mouse, hit.data) ?? hit.pane, mouse, false, false);
                }else if(hit.result == DROP) {
                    //Draggable.select(source.preDrag?.(mouse, i, id) ?? source, mouse, false, false);
                }else if(hit.result == TEXT) {
                    Editable.beginInput(hit.data[0], hit.data[1]); //haha i made hit.data = [i, object]
                }
                else {
                    //const updown = ((hit.result & TOP) == TOP) || ((hit.result & BOTTOM) == BOTTOM);
                    const updown = hit.result == TOP || hit.result == BOTTOM;
                    Draggable.select(hit.pane, mouse, updown, !updown);
                }
            }

            for(const pane of panes) {
                //if(pane.x <= mouse.x && pane.y <= mouse.y) { //bruh what i haven't been checking if it's within the size AND i've been decrementing mouse
                if(withinBounds(pane, mouse)) {
                    //mouse.x -= pane.x;
                    //mouse.y -= pane.y; //to client
                    //hit |= pane.hittest(mouse);
                    const mca = {x: mouse.x, y: mouse.y}; //mouse camera adjusted
                    if(!pane.sticky) {
                        mca.x -= camera.x;
                        mca.y -= camera.y;
                    }
                    pane.mouseDown?.(mca);
                }
            }

            if((hit.result != TEXT && hit.result != BUTTON) && Editable.editing) { //BUTTON is an exception here because there's only one time i use one and it's when you hit the new variable button in PropertyMenu
                Editable.endInput();
            }
        }else if(msg == WM_RBUTTONDOWN) {
            const mouse = {x: GET_X_LPARAM(lp)-camera.x, y: GET_Y_LPARAM(lp)-camera.y}; //lp is client mouse pos

            if(Editable.editing) {
                Editable.endInput(); //hahai have to end inpout here just in case i call BlueprintMenu.open
            }

            if(hit.result == BUTTON) {
                hit.pane.buttonRightMouseDown?.(mouse);
            }else if(hit.result == CONTEXTMENU) {
                const screenmousepos = GetCursorPos();
                hit.pane.preContextMenu?.(screenmousepos);
                TrackPopupMenu(hit.pane.contextMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, screenmousepos.x, screenmousepos.y, hwnd);
            }

            for(const pane of panes) {
                if(withinBounds(pane, mouse)) {
                    //mouse.x -= pane.x;
                    //mouse.y -= pane.y; //to client
                    //hit |= pane.hittest(mouse);
                    pane.rightMouseDown?.({x: mouse.x-pane.x, y: mouse.y-pane.y});
                }
            }
        }else if(msg == WM_LBUTTONUP) {
            ReleaseCapture();
            Draggable.mouseUp();

            const mouse = {x: GET_X_LPARAM(lp)-camera.x, y: GET_Y_LPARAM(lp)-camera.y}; //lp is client mouse pos
            //if(hit == BUTTON) {
            //    hitpane.mouseUp(mouse);
            //}
            if(activeButton) {
                activeButton.buttonUp?.(mouse);
                dirty = true;
            }
            activeButton = undefined;

            for(const pane of panes) {
                //if(pane.x <= mouse.x && pane.y <= mouse.y) {
                if(withinBounds(pane, mouse)) {
                    mouse.x -= pane.x;
                    mouse.y -= pane.y; //to client
                    //hit |= pane.hittest(mouse);
                    pane.mouseUp?.(mouse);
                }
            }
        }else if(msg == WM_COMMAND) {
            if(hit.result == CONTEXTMENU) { //mango
                hit.pane.onCommand(wp, lp);
                dirty = true;
            }
        }else if(msg == WM_KEYDOWN) {
            const ctrl = GetKey(VK_CONTROL);
            if(wp == "E".charCodeAt(0)) {
                print("e");
                if(GetKey(VK_CONTROL)) {
                    SetForegroundWindow(GetConsoleWindow());
                    try {
                        print(eval(getline("Ctrl+E -> Eval some code: ")));
                    }catch(e) {
                        print(e.toString());
                    }
                    SetForegroundWindow(hwnd);
                }
            }else if(wp == "V".charCodeAt(0) && ctrl && Editable.editing) {
                //print("think about pasting but don't actually because idk how to do anything with the clipboard yet...");
                //i've since learned...
                let success = OpenClipboard(hwnd);

                if(success) {
                    let text;
                    //if(IsClipboardFormatAvailable(CF_TEXT)) {
                    //    text = GetClipboardData(CF_TEXT);
                    //}else if(IsClipboardFormatAvailable(CF_UNICODETEXT)) {
                    //    text = GetClipboardData(CF_UNICODETEXT);
                    //}else if(IsClipboardFormatAvailable(49301)) { //if an image is copied from a chromium browser im assuming they all add the UniformResourceLocatorW format (49301)
                    //    const ptr = GetClipboardData(49301);
                    //    if(ptr) {
                    //        text = WStringFromPointer(ptr); //well hopefully this doesn't cause any crashes
                    //    }
                    //}
                    let format = GetPriorityClipboardFormat([CF_UNICODETEXT, CF_TEXT, 49301]); //oh yeah (this function checks to see if any of these formats are currently in the clipboard and chooses the first one that works)
                    if(format == 49301) { //if an image is copied from a chromium browser im assuming they all add the UniformResourceLocatorW format (49301)
                        const ptr = GetClipboardData(format);
                        if(ptr) {
                            text = WStringFromPointer(ptr); //well hopefully this doesn't cause any crashes
                        }
                    }else {
                        text = GetClipboardData(format);
                    }
                    Editable.writechar(text);

                    success = CloseClipboard();
                    if(!success) {
                        print(`CloseClipboard failed? (${g=GetLastError()} - ${_com_error(g)})`);
                    }
                }else {
                    print(`OpenClipboard failded for some reason (maybe because a window (${c=GetClipboardOwner()} - ${GetWindowText(c) }) already had it open)`, g=GetLastError(), _com_error(g));
                }
            }else if(wp == "C".charCodeAt(0) && ctrl && Editable.editing) {
                let success = OpenClipboard(hwnd);
                if(success) {
                    success = EmptyClipboard();
                    if(!success) {
                        print(`EmptyClipboard failed? (${g=GetLastError()} - ${_com_error(g)})`);
                    }

                    SetClipboardData(CF_TEXT, Editable.edited.value); //yerp (windows will automatically translate CF_TEXT into CF_OEMTEXT and CF_UNICODETEXT (to see the formats on the clipboard run scripts/clipboard.js))
                    Editable.caret = Editable.edited.value.length;

                    success = CloseClipboard();
                    if(!success) {
                        print(`CloseClipboard failed? (${g=GetLastError()} - ${_com_error(g)})`);
                    }
                }else {
                    print(`OpenClipboard failded for some reason (maybe because a window (${c=GetClipboardOwner()} - ${GetWindowText(c) }) already had it open)`, g=GetLastError(), _com_error(g));
                }
            }

            if(wp == VK_ESCAPE) {
                if(Editable.editing) Editable.endInput();
            }
            if(Editable.editing && (wp > VK_SPACE || wp <= VK_DELETE)) {
                Editable.modify(wp);
            }else if(hit.result == WHEEL && (wp == VK_UP || wp == VK_DOWN)) {
                const y = wp == VK_UP ? 120 : -120;
                if(hit.data) {
                    hit.data.wheel(y);
                }else {
                    hit.pane.wheel(y);
                }
            }
        }else if(msg == WM_CHAR) { //WM_CHAR will send the keycode of actual characters (like abc or !@#) but WM_KEYDOWN will send the keycode for every character BUT it won't send characters modified by shift (so if you hit shift+1 it will only send 1 instead of ! like WM_CHAR) also not to mention WM_KEYDOWN sends the capitalized keycode for letters (idk why)
            //so clearly i should've used WM_CHAR in jbstudio3.js
            if(Editable.editing && (wp != VK_RETURN && wp != VK_BACK && wp != 127) && wp > 31) { //for some reason ctrl+backspace puts this random character down (and windows controls always put it instead of backspacing words and it makes me mad)
                print("char", wp, String.fromCharCode(wp));
                Editable.writechar(String.fromCharCode(wp));
            }
        }/*else if(msg == WM_MBUTTONDOWN) {
            if(!Draggable.dragging) {
                SetCapture(hwnd);
                const mouse = {x: GET_X_LPARAM(lp), y: GET_Y_LPARAM(lp)}; //lp is client mouse pos
                Draggable.select(camera, mouse, false, false);
            }
        }else if(msg == WM_MBUTTONUP) {
            ReleaseCapture();
            Draggable.mouseUp();
        }*/else if(msg == WM_MOUSEWHEEL) {
            //print(wp, GET_WHEEL_DELTA_WPARAM(wp), lp); //ok no lie i had to make GET_WHEEL_DELTA_WPARAM for this
            if(hit.result == WHEEL) {
                if(hit.data) {
                    hit.data.wheel(GET_WHEEL_DELTA_WPARAM(wp));
                }else {
                    hit.pane.wheel(GET_WHEEL_DELTA_WPARAM(wp));
                }
            }
            //if(dragging) {
            //            //oh yeah dragging is a string holding either cPos or c2Pos
            //    let i = +(dragging[1] == "2"); //if dragging[1] == "2" then dragging is c2Pos meaning the radii index must be 1
            //    radii[i] += (GET_WHEEL_DELTA_WPARAM(wp)*.0001);
            //    console.log(radii);
            //    gl.uniform2fv(uniformLocations["radii"], radii);
            //}
        }/*else if(msg == WM_TIMER) { //for smooth scrolling
            //if(Math.abs(BlueprintMenu.singleton?.scrollVelocity) > 0.01) { //Math.abs(undefined) == NaN
            //    dirty = true;
            //}
            if(BlueprintMenu.singleton && Math.abs(BlueprintMenu.singleton.scrollBox.scroll.velocity) > 0.01) {
                dirty = true;
            }
        }*/
        else if(msg == WM_DESTROY) {
            for(const pane of panes) {
                pane.destroy();
            }
            print(font.Release());
            print(colorBrush.Release());
            print(d2d.Release());
            PostQuitMessage(0); //but it refused.
        }

        if(Draggable.dragging) {
            const mouse = GetCursorPos();
            ScreenToClient(hwnd, mouse);
            if(Draggable.dragged != camera) {
                mouse.x -= camera.x;
                mouse.y -= camera.y;
            }
            Draggable.update(mouse);
        }

        //print(Int_To_WM(msg), Math.random(), dirty);
        if(dirty) {
            //print("DIRTY!!!", Int_To_WM(msg), Math.random());
            this.paint();
            dirty = false;
            draws++;
        }

        return returnvalue;
    }
}

globalThis.Draggable = Draggable;
globalThis.Editable = Editable;
globalThis.Gradient = Gradient;
globalThis.GenericControl = GenericControl;
globalThis.StaticControl = StaticControl;
globalThis.WatchStaticControl = WatchStaticControl;
globalThis.CheckboxControl = CheckboxControl;
globalThis.EditControl = EditControl;
globalThis.TextButtonControl = TextButtonControl;
globalThis.DropdownControlMenu = DropdownControlMenu;
globalThis.EditDropdownControl = EditDropdownControl;
globalThis.DropdownButtonControl = DropdownButtonControl;
globalThis.DropdownList = DropdownList;
globalThis.VerticalScrollBox = VerticalScrollBox;
globalThis.HorizontalListBox = HorizontalListBox;
globalThis.ImageButtonControl = ImageButtonControl;
globalThis.Base = Base;
globalThis.Parent = Parent;
globalThis.D2DWindow = D2DWindow;