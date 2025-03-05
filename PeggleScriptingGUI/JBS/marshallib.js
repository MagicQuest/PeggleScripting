//well since i want to use this js file with other files i gotta eval it but since you can't declare new variables with eval im using globalThis
globalThis.sizeof = {
    CHAR : 1,
    BYTE : 1,
    UCHAR : 1, //BYTE is UCHAR
    WORD : 2, //WORD is unsigned short
    USHORT: 2,
    UINT : 4,
    INT : 4,
    FLOAT : 4,
    DWORD : 4,
    COLORREF : 4, //same as DWORD
    LONG : 4,
    ULONG : 4,
    ULONGLONG: 8,
    UINT_PTR: 8,
    ULONG_PTR : 8,
    LONG_PTR : 8,
    DOUBLE: 8,
    HANDLE : 8,
    HWND : 8,
};

globalThis.additionaltypedata = {
    BYTE: {unsigned: true},
    UCHAR: {unsigned: true},
    WORD: {unsigned: true},
    USHORT: {unsigned: true},
    UINT: {unsigned: true},
    ULONG: {unsigned: true},
    ULONGLONG: {unsigned: true},
    UINT_PTR: {unsigned: true},
    ULONG_PTR: {unsigned: true},
    DWORD: {unsigned: true},
    FLOAT: {floating: true},
    DOUBLE: {floating: true},
}

function defineProps(obj, data, key, i, datatype, bytes) {
    const reg = /([A-Za-z0-9]{2})/g;
    if(datatype.prototype instanceof memoobjectidk) {
        let none = {constructor: datatype};
        Object.defineProperty(obj, key, {
            get() {
                return none;
            },
            set(newvalue) {
                print("no setting yet lol")
            },
            enumerable: true,
            configurable: true,
        });
        objFromTypes(none, data, i);
        delete none.constructor; //lol objFromTypes checks the obj's constructor (and i don't want to make an instance of datatype)
        return; //i think
    }
    
    Object.defineProperty(obj, key, { //windows is little endian!!!
        get() {
            const dataview = new DataView(data.buffer, i, bytes);

            if(additionaltypedata[datatype]) {
                if(additionaltypedata[datatype].floating) {
                    if(bytes == 4) {
                        return dataview.getFloat32(0, true);
                    }else if(bytes == 8) {
                        return dataview.getFloat64(0, true);
                    }
                }else if(additionaltypedata[datatype].unsigned) {
                    if(bytes == 1) {
                        return dataview.getUint8(0, true);
                    }else if(bytes == 2) {
                        return dataview.getUint16(0, true);
                    }else if(bytes == 4) {
                        return dataview.getUint32(0, true);
                    }else if(bytes == 8) {
                        const v = dataview.getBigUint64(0, true);
                        if(v > 2n**53n) {
                            print(`lowkey casting bigint back to js number which will lose precision if bigger than 2**53 (${2**53})`);
                            print(`your value is`, v);
                        }
                        return Number(v); //sorry lol but not being able to mix bigint and int is a dealbreaker for bigint
                    }
                }
            }else {
                if(bytes == 1) {
                    return dataview.getInt8(0, true);
                }else if(bytes == 2) {
                    return dataview.getInt16(0, true);
                }else if(bytes == 4) {
                    return dataview.getInt32(0, true);
                }else if(bytes == 8) {
                    const v = dataview.getBigInt64(0, true);
                    if(v > 2n**53n) {
                        print(`lowkey casting bigint back to js number which will lose precision if bigger than 2**53 (${2**53})`);
                        print(`your value is`, v);
                    }
                    return Number(v); //sorry lol but not being able to mix bigint and int is a dealbreaker for bigint
                }
            }
        },
        set(newValue) {
            const dataview = new DataView(data.buffer, i, bytes);

            if(additionaltypedata[datatype]) {
                if(additionaltypedata[datatype].floating) {
                    if(bytes == 4) {
                        dataview.setFloat32(0, newValue, true);
                    }else if(bytes == 8) {
                        dataview.setFloat64(0, newValue, true);
                    }
                }else if(additionaltypedata[datatype].unsigned) {
                    if(bytes == 1) {
                        dataview.setUint8(0, newValue, true);
                    }else if(bytes == 2) {
                        dataview.setUint16(0, newValue, true);
                    }else if(bytes == 4) {
                        dataview.setUint32(0, newValue, true);
                    }else if(bytes == 8) {
                        //dataview.setBigUint64(0, BigInt(newValue)), true; //how tf did i make this mistake LMAO vscode wasn't saying ANYTHING
                        dataview.setBigUint64(0, BigInt(newValue), true);
                    }
                }
            }else {
                if(bytes == 1) {
                    dataview.setInt8(0, newValue, true);
                }else if(bytes == 2) {
                    dataview.setInt16(0, newValue, true);
                }else if(bytes == 4) {
                    dataview.setInt32(0, newValue, true);
                }else if(bytes == 8) {
                    dataview.setBigInt64(0, BigInt(newValue), true);
                }
            }
        },
        enumerable: true,
        configurable: true,
    });

    //i sat here and reinvented the wheel and you're telling me that a DataView object could already do this?!!!
    //Object.defineProperty(obj, key, {
    //    get() { //THIS SHIT IS LITTLE ENDIAN!
    //        //if(datatype instanceof memoobjectidk) {
    //        //    //uhhh i guess don't really do anything special
    //        //    return datatype;
    //        //}else {
    //            let fulltype = 0n;
    //            //wait i can automate all this
    //            for(let j = 0; j < bytes; j++){
    //                //print(i, j, bytes, data[i+j], key, datatype);
    //                //oh windows is little esndian
    //                //fulltype |= data[i+j] << (bytes-1-j)*8; //oh hell yeah this shit looks good :drooling: (whatchu you know about rolling dowjn in the deep when your brain goes numb (another medium: https://www.youtube.com/watch?v=DXuOAWnuMVY))
    //                fulltype |= BigInt(data[i+j]) << BigInt(j)*8n; //ok here we go (oops when the numbers go too big js couldn't keep up and would fuck em up i gotta use bigint)
    //            }
    //
    //            fulltype = Number(fulltype);
    //
    //            //if(datatype[0] == "U") {
    //            if(additionaltypedata[datatype]?.unsigned || datatype[0] == "U") {
    //                return fulltype < 0 ? (2**(bytes*8))-fulltype : fulltype; //checking if it's unsigned and flipping it 
    //            }else {
    //                //print(fulltype, (2**(bytes*8-1)));
    //                return fulltype >= (2**(bytes*8-1)) ? fulltype-(2**(bytes*8)) : fulltype;
    //            }
    //        //}
    //    },
    //    set(newValue) { //oops idk if i wrote the get part wrong or you aren't allowed to set signed correctly (going over doesn't flip to negative) (hey look at that i fixed it)
    //        /*
    //        if(datatype[0] == "U") {
    //            //newValue = newValue < 0 ? (2**(bytes*8))-newValue : newValue;
    //            if(newValue < 0) {
    //                newValue += (2**(bytes*8));
    //            }else if(newValue >= 2**(bytes*8)) {
    //                newValue -= (2**(bytes*8));
    //            }
    //        }else {
    //            if(newValue < 0) {
    //                newValue += (2**(bytes*8));
    //            }else if(newValue >= 2**(bytes*8-1)) {
    //                newValue -= (2**(bytes*8));
    //            }
    //        }
    //        */
    //        //if(newValue < 0) {
    //        //    newValue += 2**(bytes*8);
    //        //}else if(datatype[0] == "U" && newValue >= 2**(bytes*8)) {
    //        //    //newValue = newValue < 0 ? (2**(bytes*8))-newValue : newValue;
    //        //    newValue -= 2**(bytes*8);
    //        //}else if(newValue >= 2**(bytes*8-1)) { //oops i had it 2**(bytes*8-1) but it wasn't working 100% of the time and i actually didn't even need this branch...
    //        //    newValue -= 2**(bytes*8);
    //        //}
    //        if(newValue < 0) {
    //            newValue += 2**(bytes*8);
    //        }else if(newValue >= 2**(bytes*8)) {
    //            newValue -= 2**(bytes*8);
    //        }
    //        let hex = newValue.toString(16);
    //        //for(let j = 0; j < hex.length; j++) { //bruh i accidently was creating an infinite loop here but for some reason v8 would instantly crash (i had to test this hoe on another website bruh)
    //        //    hex = "00"+hex;
    //        //}
    //        if(hex.length % 2 == 1) {
    //            hex = "0"+hex;
    //        }
    //        const hl = hex.length;
    //        for(let j = 0; j < bytes-(hl/2); j++) {
    //            hex = "00"+hex;
    //        }
    //        //console.log(hex.length, bytes*2);
    //        //if(hex.length > bytes*2) {
    //        //    console.log("too large nigga");
    //        //    hex = Object.keys(new Array(bytes*2).toString()+1).map(e => "00").join(""); //bruh i KNOW i had some bullshit like this somewhere and i spent like 20 minutes trying to find it and i couldn't :(   (where would i have possibly put a function like this (equivalent to python's range or something idk))
    //        //}
    //        //print(bytes, hex, i);
    //
    //        const hexBytes = hex.match(reg);
    //        for(let j = 0; j < bytes; j++) {
    //            //data[i+j] = parseInt(hexBytes[j], 16);
    //            //console.log(i, j, hexBytes[bytes-1-j])
    //            data[i+j] = parseInt(hexBytes[bytes-1-j], 16); //swizzle
    //        }
    //    },
    //    enumerable: true,
    //    configurable: true,
    //});
}

//class memoobjectidk { //i was really trying to make this inheritance work but i just can't seem to modify the child and read this.types from it (i was doin' it wrong)
//ok lowkey if you try to initialize an object and it has a special type (another memoobjectidk class) it won't set it the way it should (it will tell you that you can't set it like that yet lol)
globalThis.objFromTypes = (obj, data, offset=0, listinit=false, vargs) => { //replacing every this. with obj.
    //types = {};
    //constructor(data) {
        let addr = offset;
        let indexforlistinit = 0; //for listinit
        for(const key in obj.constructor.types) { //using constructor now because types is static
            const datatype = obj.constructor.types[key];
            let bytes;
            const arrlength = obj.constructor.arrayLengths?.[key] ?? 1;
            if(datatype.prototype instanceof memoobjectidk) {
                bytes = datatype.sizeof()*arrlength;
            }else {
                bytes = sizeof[datatype]*arrlength;
            }
            if(bytes == undefined) {
                print(key, datatype, ":fucked up");
            }
            //let fulltype = 0;
            ////wait i can automate all this
            //for(let j = 0; j < bytes; j++){
            //    //oh windows is little esndian
            //    //fulltype |= data[i+j] << (bytes-1-j)*8; //oh hell yeah this shit looks good :drooling: (whatchu you know about rolling dowjn in the deep when your brain goes numb (another medium: https://www.youtube.com/watch?v=DXuOAWnuMVY))
            //    fulltype |= data[i+j] << (j)*8; //ok here we go
            //}
            ////if(bytes == 1) { //CHAR/UCHAR
            ////    fulltype = data[i];
            ////}else if(bytes == 4) { //INT/UINT
            ////    fulltype = (data[i] << 24) | (data[i+1] << 16) | (data[i+2] << 8) | (data[i+3]); //i think?
            ////}
            //if(datatype[0] == "U") {
            //    obj[key] = fulltype < 0 ? (2**(bytes*8))-fulltype : fulltype; //checking if it's unsigned and flipping it 
            //}else {
            //    obj[key] = fulltype >= (2**(bytes*8)) ? fulltype-(2**(bytes*8)) : fulltype;
            //}
            if(obj.constructor.arrayLengths?.[key]) {
                let arr = [];
                let offset = 0;
                const amount = datatype.sizeof?.() ?? sizeof[datatype];
                for(let i = 0; i < obj.constructor.arrayLengths[key]; i++) {
                    defineProps(arr, data, i, addr+offset, datatype, amount); //haha lol i accidently left bytes here instead of amount and i was getting totally weird errors (jbs would crash when printing IMAGE_DOS_HEADERS!)
                    if(listinit && vargs[indexforlistinit]) {
                        arr[i] = vargs[indexforlistinit]; //yeah probably
                    }
                    indexforlistinit++;
                    offset += amount;
                }
                obj[key] = arr;
            }else {
                defineProps(obj, data, key, addr, datatype, bytes);
            }
            if(listinit && arrlength == 1 && vargs[indexforlistinit]) {
                obj[key] = vargs[indexforlistinit];
            }
            addr += bytes; //i guess you could call this addr too
            indexforlistinit++; //for listinit
        }
    //}
    //obj.sizeof = (function() {
    //    print(this);
    //    return Object.values(this.types).reduce((a, b) => a+sizeof[b], 0);
    //}).bind(obj); //haha bind! very fun! (this seems sarcastic but it's not lmao)
}

globalThis.__asm = function(arr, argc = 0, argv = [], argtypev = [], returntype = RETURN_NUMBER) {
    const asm = arr.constructor.name == "Uint8Array" ? arr : new Uint8Array(arr);

    //this ptr variable is the address of a portion of memory allocated by JS/V8 for my arraybuffer asm
    //this piece of memory can't be executed because of its memory protection flags type shit
    //so im gonna change that shit
    const ptr = PointerFromArrayBuffer(asm);

    //old is the previous memory protection flags
    const old = VirtualProtect(ptr, asm.byteLength, PAGE_EXECUTE_READWRITE);

    //apparently when changing code in memory you are supposed to use FlushInstructionCache but i might not have to because im not CHANGING code im making it (lowkey don't trust me on that)
    //FlushInstructionCache(hInstance, ptr, asm.byteLength); //im not sure if hInstance is also a valid process handle

    const res = Call(ptr, argc, argv, argtypev, returntype);
    VirtualProtect(ptr, asm.byteLength, old); //just in case
    return res;
}

const changeEndianness = (string) => { //https://stackoverflow.com/questions/5320439/how-do-i-swap-endian-ness-byte-order-of-a-variable-in-javascript
    const result = [];
    let len = string.length - 2;
    while (len >= 0) {
      result.push(string.substr(len, 2));
      len -= 2;
    }
    return result.join('');
}

globalThis.int32_to_little_endian_hex = function(int32) {
    let hex = int32.toString(16);
    if(hex.length % 2 == 1) {
        hex = "0"+hex;
    }
    const len = hex.length;
    for(let i = len; i < 8; i++) {
        hex = "0"+hex;
    }
    //print(hex);
    return changeEndianness(hex).match(/([a-z0-9]{2})/g).map(e => parseInt(e, 16));
}

globalThis.int64_to_little_endian_hex = function(int) {
    let hex = int.toString(16);
    if(hex.length % 2 == 1) {
        hex = "0"+hex;
    }
    const len = hex.length;
    for(let i = len; i < 16; i++) {
        hex = "0"+hex;
    }
    //print(hex);
    return changeEndianness(hex).match(/([a-z0-9]{2})/g).map(e => parseInt(e, 16));
}

//boy i feel like this would be a great time to use a template if this were c++
//also this function is a more general version of dllstuffs/ffid2d.js's dereferenceULONG_PTR function
globalThis.dereference = function(ptr, datatype, index = 0) { //returns *(ULONG_PTR*)ptr
    const special = datatype.prototype instanceof memoobjectidk;
    let size = special ? datatype.sizeof() : sizeof[datatype];
    print("dereferencing", ptr+(index*size));
    if(ptr == 0) {
        print("dereferencing NULL ptr!\x07");
        return 0;
    }
    if(additionaltypedata[datatype]?.floating) { //ouuuuhh we gotta use the xmm0 register to return a float!
        //do i gotta clear xmm0 first or do they both clear the whole thing? (ok im gonna assume i should clear it)
        const opcodes = [
            0x0f, 0x57, 0xc0, //xorps xmm0, xmm0    ;clear xmm0 and set it to 0
        ];
        if(datatype == "FLOAT") { //4
            opcodes.push(0xf3, 0x0f, 0x10, 0x01); //movss xmm0, DWORD PTR [rcx]    ;yo i had no idea how to do any floating point bullshit (look i tried to figure it out myself but in the end i had to open godbolt)
        }else if(datatype == "DOUBLE") { //8
            opcodes.push(0xf2, 0x0f, 0x10, 0x01); //movsd xmm0, QWORD PTR [rcx]    ;mov double precision floating-point value
        }
        opcodes.push(0xc3); //ret
        return __asm(opcodes, 1, [ptr+(index*size)], [VAR_INT], RETURN_DOUBLE);
    }else if(special) {
        const dereferencedPtr = dereference(ptr+(index*size), "ULONGLONG");
        const typeinstance = new datatype();
        memcpy(PointerFromArrayBuffer(typeinstance.data), dereferencedPtr, size);
        return typeinstance;
    }else {
        const opcodes = [
            0x31, 0xc0, //xor eax, eax (just in case)
        ];
        if(size == 1) {
            opcodes.push(0x8a, 0x01); //mov al, BYTE PTR [rcx]
        }else if(size == 2) {
            opcodes.push(0x66, 0x8b, 0x01); //mov ax, WORD PTR [rcx]
        }else if(size == 4) {
            opcodes.push(0x8b, 0x01); //mov eax, DWORD PTR [rcx]
        }else if(size == 8) {
            opcodes.push(0x48, 0x8b, 0x01); //mov rax, QWORD PTR [rcx];
        }
        opcodes.push(0xc3); //ret
        return __asm(opcodes, 1, [ptr+(index*size)], [VAR_INT], RETURN_NUMBER);
    }
}

//say goodbye to weird constructors like
//new ULONG_PTR(new Uint8Array(ULONG_PTR.sizeof()));
//memoobjectidk will do that for you if is not a Uint8Array and you can now initialize properties!
class memoobjectidk {
    static sizeof() {
        return Object.entries(this.types).reduce((a, [key, b]) => {
            const arrlength = this.arrayLengths?.[key] ?? 1;
            if(b.prototype instanceof memoobjectidk) {
                return a+b.sizeof()*arrlength;
            }else {
                return a+sizeof[b]*arrlength;
            }
        }, 0); //oh yeah
    }
    setData(maybedata) {
        let data = maybedata;
        if(!data) {
            data = new Uint8Array(this.constructor.sizeof()); //this is valid? calling constructor.sizeof still returns the right values and not just undefined?
        }
        objFromTypes(this, data, 0);
        this.data = data;
    }
    constructor(maybedata, ...vargs) {
        let data = maybedata;
        let listinit = !(maybedata instanceof Uint8Array); //if you pass something other than a Uint8Array for data i will assume you are trying to emulate list initialization
        if(listinit) {
            data = new Uint8Array(this.constructor.sizeof()); //this is valid? calling constructor.sizeof still returns the right values and not just undefined?
        }
        objFromTypes(this, data, 0, listinit, [maybedata, ...vargs]);
        this.data = data;
    }
}
class CString {
    constructor(string) {
        this._data = new Uint8Array([...string.split("").map(e => e.charCodeAt(0)), 0x00]);
        this.ptr = PointerFromArrayBuffer(this._data);
    }

    get value() {
        //return String.fromCharCode(...Object.values(this._data));
        let str = "";
        for(let i = 0; i < this._data.length; i++) {
            if(this._data[i] == 0) {
                break;
            }
            str += String.fromCharCode(this._data[i]);
        }
        return str;
    }
    set value(newValue) {
        if(newValue.length >= this._data.length) {
            //too big and i gotta make a new array
            //print("too big", newValue.length, ">=", this._data.length);
            this._data = new Uint8Array([...newValue.split("").map(e => e.charCodeAt(0)), 0x00]);
            this.ptr = PointerFromArrayBuffer(this._data);
        }else {
            for(let i = 0; i < this._data.length; i++) {
                if(newValue.length <= i) {
                    this._data[i] = 0;
                }else {
                    this._data[i] = newValue[i].charCodeAt(0);
                }
            }
        }
    }
}
class WString {
    constructor(wstring) {
        this._data = new Uint8Array([...wstring.split("").map(e => e+"\0").join("").split("").map(e => e.charCodeAt(0)), 0x00, 0x00]) //now that LoadIcon takes wchar_t i gotta add zeros because wchar is 2 bytes
        this.ptr = PointerFromArrayBuffer(this._data);
    }

    get value() {
        //return String.fromCharCode(...Object.values(this._data));
        let str = "";
        for(let i = 0; i < this._data.length; i += 2) {
            if(this._data[i] == 0) {
                break;
            }
            str += String.fromCharCode(this._data[i]);
        }
        return str;
    }
    set value(newValue) {
        if(newValue.length >= this._data.length/2) {
            //too big and i gotta make a new array
            print("too big", newValue.length, ">=", this._data.length/2);
            this._data = new Uint8Array([...newValue.split("").map(e => e+"\0").join("").split("").map(e => e.charCodeAt(0)), 0x00, 0x00]);
            this.ptr = PointerFromArrayBuffer(this._data);
        }else {
            for(let i = 0; i < this._data.length; i++) {
                if(i % 2 == 1 || newValue.length*2 <= i) {
                    this._data[i] = 0;
                }else {
                    this._data[i] = newValue[(i/2)].charCodeAt(0);
                }
            }
        }
    }
}
globalThis.memoobjectidk = memoobjectidk;
globalThis.CString = CString;
globalThis.WString = WString;