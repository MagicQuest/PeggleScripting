eval(require("fs").read(__dirname+"/mastercontrols.js"));
print("lowkey we need marshallib so im checking for it in ../dllstuffs/marshallib.js");
eval(require("fs").read(__dirname+"/../dllstuffs/marshallib.js"));

//i've actually already defined these objects back in /dllstuffs/ffigetexportedfunctions.js
//these objects are for the LocateAndGetProcAddressEx function

class IMAGE_DOS_HEADER extends memoobjectidk {
    static types = {
        e_magic: "WORD",
        e_cblp: "WORD",
        e_cp: "WORD",
        e_crlc: "WORD",
        e_cparhdr: "WORD",
        e_minalloc: "WORD",
        e_maxalloc: "WORD",
        e_ss: "WORD",
        e_sp: "WORD",
        e_csum: "WORD",
        e_ip: "WORD",
        e_cs: "WORD",
        e_lfarlc: "WORD",
        e_ovno: "WORD",
        e_res: "WORD", //[4]
        e_oemid: "WORD",
        e_oeminfo: "WORD",
        e_res2: "WORD", //[10]
        e_lfanew: "LONG",
    }
    static arrayLengths = {
        e_res: 4,
        e_res2: 10,
    }
    constructor(data, ...vargs) { //data must be a Uint8Array
        super(data, ...vargs);
    }
}

class IMAGE_FILE_HEADER extends memoobjectidk {
    static types = {
        Machine: "WORD",
        NumberOfSections: "WORD",
        TimeDateStamp: "DWORD",
        PointerToSymbolTable: "DWORD",
        NumberOfSymbols: "DWORD",
        SizeOfOptionalHeader: "WORD",
        Characteristics: "WORD",
    };
    constructor(data, ...vargs) { //data must be a Uint8Array
        super(data, ...vargs);
    }
}

class IMAGE_DATA_DIRECTORY extends memoobjectidk {
    static types = {
        VirtualAddress: "DWORD",
        Size: "DWORD",
    }
    constructor(data, ...vargs) { //data must be a Uint8Array
        super(data, ...vargs);
    }
}

const IMAGE_NUMBEROF_DIRECTORY_ENTRIES = 16;

class IMAGE_OPTIONAL_HEADER32 extends memoobjectidk {
    static types = {
        Magic: "WORD",
        MajorLinkerVersion: "BYTE",
        MinorLinkerVersion: "BYTE",
        SizeOfCode: "DWORD",
        SizeOfInitializedData: "DWORD",
        SizeOfUninitializedData: "DWORD",
        AddressOfEntryPoint: "DWORD",
        BaseOfCode: "DWORD",
        BaseOfData: "DWORD",

        ImageBase: "DWORD",
        SectionAlignment: "DWORD",
        FileAlignment: "DWORD",
        MajorOperatingSystemVersion: "WORD",
        MinorOperatingSystemVersion: "WORD",
        MajorImageVersion: "WORD",
        MinorImageVersion: "WORD",
        MajorSubsystemVersion: "WORD",
        MinorSubsystemVersion: "WORD",
        Win32VersionValue: "DWORD",
        SizeOfImage: "DWORD",
        SizeOfHeaders: "DWORD",
        CheckSum: "DWORD",
        Subsystem: "WORD",
        DllCharacteristics: "WORD",
        SizeOfStackReserve: "DWORD",
        SizeOfStackCommit: "DWORD",
        SizeOfHeapReserve: "DWORD",
        SizeOfHeapCommit: "DWORD",
        LoaderFlags: "DWORD",
        NumberOfRvaAndSizes: "DWORD",
        DataDirectory: IMAGE_DATA_DIRECTORY,
        //DataDirectory0: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory1: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory2: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory3: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory4: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory5: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory6: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory7: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory8: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory9: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory10: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory11: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory12: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory13: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory14: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
        //DataDirectory15: IMAGE_DATA_DIRECTORY, //[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] (not sure how i'll do arrays)
    }
    static arrayLengths = {
        DataDirectory: IMAGE_NUMBEROF_DIRECTORY_ENTRIES,
    }
    constructor(data, ...vargs) { //data must be a Uint8Array
        super(data, ...vargs);
    }
}

class IMAGE_NT_HEADERS32 extends memoobjectidk {
    //oh shoot these types have to be in the right order
    static types = {
        Signature: "DWORD",
        //IMAGE_FILE_HEADER
        FileHeader: IMAGE_FILE_HEADER,
        //IMAGE_OPTIONAL_HEADER64
        OptionalHeader: IMAGE_OPTIONAL_HEADER32,
    };
    constructor(data, ...vargs) { //data must be a Uint8Array
        super(data, ...vargs);
    }
}

class IMAGE_EXPORT_DIRECTORY extends memoobjectidk {
    static types = {
        Characteristics: "DWORD",
        TimeDateStamp: "DWORD",
        MajorVersion: "WORD",
        MinorVersion: "WORD",
        Name: "DWORD",
        Base: "DWORD",
        NumberOfFunctions: "DWORD",
        NumberOfNames: "DWORD",
        AddressOfFunctions: "DWORD",     // RVA from base of image
        AddressOfNames: "DWORD",         // RVA from base of image
        AddressOfNameOrdinals: "DWORD",  // RVA from base of image
    };
    constructor(data, ...vargs) { //data must be a Uint8Array
        super(data, ...vargs);
    }
}

function NewPtrArray(type, length) { //yeah this is kinda weird but you can't really change types or arrayLenghts after you make the class since its static
    class PtrArray extends memoobjectidk {
        static types = {
            //values: "LONG_PTR",  //i assume?
            values: type, //wait why was it DWORD and not LONG_PTR? (oh wait in the stack overflow post they cast names[i] to an int)
        };
        static arrayLengths = {
            values: length,
        };
        constructor(data, ...vargs) { //data must be a Uint8Array
            super(data, ...vargs);
        }
    }
    return new PtrArray(new Uint8Array(PtrArray.sizeof())); //you don't have to initialize a memoobjectidk like this anymore because if data is undefined it will do it for you!
}

let w = 800;
let h = 600;

let wic, d2d, colorBrush, font, roundStrokeStyle; //required!

class Bitmap {
    //visible = true;

    constructor(x, y, width, height, path, opacity = 1.0) {
        this.transform(x, y, width, height);
        this.img = d2d.CreateBitmapFromWicBitmap(wic.LoadBitmapFromFilename(path, wic.GUID_WICPixelFormat32bppPBGRA), true);
        this.opacity = opacity;
        this.rotation = 0;
    }

    transform(x, y, width, height) {
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
    }

    resize(newwidth, newheight) {
        this.width = newwidth;
        this.height = newheight;
    }

    redraw() {
        //if(this.visible) {
            const prev = d2d.GetTransform(); //i should be able to just do prev.Translation() or something so i can add it so i might have to add that
            //const copy = Object.assign({}, prev); //hell yeah brother next best thing to structuredClone
            //print(copy);
            //copy.dx += x; //i mean probably dawg unfortunately matrices are outta my league currently
            //copy.dy += y;
            ////oops wrong properties
            //copy.m[2][0] += x;
            //copy.m[2][1] += y;
            // const transform = Matrix3x2F.Rotation(this.rotation, prev._31+this.x, prev._32+this.y);
            const transform = Matrix3x2F.Multiply(Matrix3x2F.Translation(prev._31+this.x, prev._32+this.y), Matrix3x2F.Rotation(this.rotation, prev._31+this.x+this.width/2, prev._32+this.y+this.height/2));
            // const copy = Matrix3x2F.Rotation(this.rotation, 0, 0);
            // copy._31 += this.x;
            // copy._32 += this.y;
            //copy._31 += this.x; //ok idk why Matrix3x2F has so many useless properties since this is the only one i had to change
            //copy._32 += this.y;
            //print(copy, "POST COPY");
            d2d.SetTransform(transform);
            d2d.DrawBitmap(this.img, 0, 0, this.width, this.height, this.opacity);
            d2d.SetTransform(prev);
        //}
    }

    destroy() {
        this.img.Release();
    }
}

class GenericTiledBitmap extends Bitmap {
    constructor(x, y, width, height, path, opacity, modes, i = 0) {
        super(x, y, width, height, path, opacity);
        this.modes = modes;
        this.i = i;
    }

    redraw() {
        //if(this.visible) {
            const {x: sx, y: sy, width: sw, height: sh} = this.modes[this.i]; //insane how i didn't know this was a thing until really recently
            d2d.DrawBitmap(this.img, this.x, this.y, this.x+this.width, this.y+this.height, this.opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, sx, sy, sx+sw, sy+sh);
        //}
    }
}

class TiledBitmapButton extends GenericControl {
    alwaysHittest = true;
    visible = true;
    down = false;

    constructor(bitmap, callback, parent) {
        super(bitmap.x, bitmap.y, bitmap.width, bitmap.height);
        this.bitmap = bitmap;
        this.callback = callback.bind(parent);
    }

    transform(x, y, newwidth, newheight) {
        this.x = x;
        this.y = y;
        super.resize(newwidth, newheight);
        this.bitmap.transform(x, y, newwidth, newheight);
    }

    redraw() {
        // print("redraw", this.bitmap.i);
        if(this.visible) {
            this.bitmap.redraw();
        }
    }

    buttonDown() {
        //this.callback();
    }

    resize(newwidth, newheight) {
        super.resize(newwidth, newheight);
        this.down = false;
    }

    mouseDown(mouse) {
        if(withinBounds(this, mouse)) {
            this.bitmap.x += 1;
            this.bitmap.y += 1;
            this.down = true;
            PlaySoundSpecial(__dirname+"/psgui/button.mp3", "buttonclick");
        }
    }

    mouseUp(mouse) {
        if(withinBounds(this, mouse) && this.down) {
            this.bitmap.x -= 1;
            this.bitmap.y -= 1;
            this.down = false;
            this.callback();
        }
    }

    hittest(mouse) {
        // print("hittest BUDDY what?",this.bitmap.i);
        this.bitmap.i = 0;
        //dirty = true; //i have no idea why but this was stopping my shit from working
        if(this.visible) {
            if(withinBounds(this, mouse)) {
                this.bitmap.i = 1;
                return [BUTTON, this];
            }else if(this.down) {
                this.down = false;
                this.bitmap.x -= 1;
                this.bitmap.y -= 1;
            }
        }
    }

    destroy() {
        this.bitmap.destroy();
    }
}

function hProcessFromHWND(hwnd) { //snatched from the ol'e memoview.js
    const {processID} = GetWindowThreadProcessId(hwnd);
    if(!processID) {
        const g = GetLastError();
        print("GetWindowThreadProcessId failed\x07", g, _com_error(g));
    }
    let hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processID); //OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, false, processID); //oh lol i forgot query!
    if(!hProcess) {
        const g = GetLastError();
        print(`OpenProcess failed (what was it tho ${hProcess})\x07`, g, _com_error(g));
    }
    return hProcess;
}

function errMessage(msg) {
    PlaySoundSpecial(__dirname+"/psgui/tone.mp3", "tone", this.hwnd);
    const g = GetLastError();
    print(msg+"\x07", g, _com_error(g));
    Msgbox(msg, _com_error(g), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
}

function LocateAndGetProcAddressEx(hProcess, moduleName, procName) {
    const pid = GetProcessId(hProcess);
    const snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid) //the PeggleScripting.dll also uses this function to find the base of popcapgame1.exe
    if(snapshot == INVALID_HANDLE_VALUE) {
        //this.hookErrCheck("CreateToolhelp32Snapshot");
        errMessage("(LocateAndGetProcAddressEx) CreateToolhelp32Snapshot failed!");
        return;
    }
    
    let me32 = Module32First(snapshot);
    if(!me32) {
        //this.hookErrCheck("Module32First failed!", snapshot);
        errMessage("(LocateAndGetProcAddressEx) Module32First failed!");
        return;
    }

    do {
        print("MODULE NAME:", me32.szModule);
        print("executable    = ", me32.szExePath);
        print("process id    = ", me32.th32ProcessID);
        print("ref count (g) = ", me32.GlblcntUsage);
        print("ref count (p) = ", me32.ProccntUsage);
        print("base address  = ", me32.modBaseAddr);
        print("base size     = ", me32.modBaseSize);
        if(me32.szModule.toLowerCase().includes(moduleName)) {
            //ejhll euah
            print(`${moduleName} found!`); //now how do get proc address from another process (maybe i'll just copy the header and read through it)
            break;
        }
    }while(Module32Next(snapshot, me32)); //directly modifies me32!!!!!!!!!

    CloseHandle(snapshot);
    
    //yeah i think im gonna have to read the headers myself to find the function's address
    //damn we actually might need marshallib.js
    const DOS = new IMAGE_DOS_HEADER(ReadProcessMemory(hProcess, me32.modBaseAddr, IMAGE_DOS_HEADER.sizeof()));
    //skip past the ms dos stub
    const NT = new IMAGE_NT_HEADERS32(ReadProcessMemory(hProcess, me32.modBaseAddr+DOS.e_lfanew, IMAGE_NT_HEADERS32.sizeof()));
    
    print(NT.OptionalHeader.DataDirectory[0]);
    const exportDirectoryRVA = NT.OptionalHeader.DataDirectory[0].VirtualAddress;
    const entireExportDirectory = ReadProcessMemory(hProcess, me32.modBaseAddr+exportDirectoryRVA, NT.OptionalHeader.DataDirectory[0].Size); //bruh i accidently used a lower case s...
    const EXPORT_DIRECTORY = new IMAGE_EXPORT_DIRECTORY(ReadProcessMemory(hProcess, me32.modBaseAddr+exportDirectoryRVA, IMAGE_EXPORT_DIRECTORY.sizeof()));
    //print("EXP:", EXPORT_DIRECTORY);
    //print("ENTIRE:",entireExportDirectory);
    
    const nameslistaddress = me32.modBaseAddr+EXPORT_DIRECTORY.AddressOfNames;
    const namesarray = NewPtrArray("INT", EXPORT_DIRECTORY.NumberOfNames);
    namesarray.setData(ReadProcessMemory(hProcess, nameslistaddress, namesarray.data.byteLength));

    const functionslistaddress = me32.modBaseAddr+EXPORT_DIRECTORY.AddressOfFunctions;
    const functionsarray = NewPtrArray("INT", EXPORT_DIRECTORY.NumberOfFunctions);
    functionsarray.setData(ReadProcessMemory(hProcess, functionslistaddress, functionsarray.data.byteLength));
    // print(functionslistaddress, functionsarray);

    const ordinalslistaddress = me32.modBaseAddr+EXPORT_DIRECTORY.AddressOfNameOrdinals;
    const ordinalsarray = NewPtrArray("WORD", EXPORT_DIRECTORY.NumberOfNames); //SHORT
    ordinalsarray.setData(ReadProcessMemory(hProcess, ordinalslistaddress, ordinalsarray.data.byteLength));

    //print(nameslist, namesarray);
    let procOrdinal;
    //oh boy this is getting harder to understand since i have to ReadProcessMemory every time :| (but i COULD just read the ENTIRE module)
    //my boy i have no idea what just happened but this loop almost crashed my computer
    //it sucked all my memory up dawg what did i just write?????
    for(let i = 0; i < EXPORT_DIRECTORY.NumberOfNames; i++) {
        const nameptr = me32.modBaseAddr+namesarray.values[i]; //the actual address of the name in memory (but we don't know how long it is)
        //ummm idk how big it's gonna be
        //is there a VirtualStrlenEx ?
        //ok i'll just read the ENTIRE export directory and then do some offset magic on that
        const offsetMagic = (nameptr-me32.modBaseAddr) - (exportDirectoryRVA);
        //print(nameptr, offsetMagic);
        //__debugbreak();
        let str = "";                       //bigger than 0 here because i said so
        for(let j = offsetMagic; entireExportDirectory[j] > 0; j++) { //im not sure if i've seen a for loop that does something like this before
            str += String.fromCharCode(entireExportDirectory[j]);
        }
        print(str, i, ordinalsarray.values[i]);
        if(str == procName) {
            procOrdinal = ordinalsarray.values[i];
            // procindex = i; //wait no i need the ordinal not i!
            break;
        }
    }

    if(procOrdinal != undefined) {
        print(procOrdinal, functionsarray.values[procOrdinal]);
        return me32.modBaseAddr+functionsarray.values[procOrdinal];
    }
}

function lerp(a, b, t) {
	return t * (b - a) + a; //ima geinus
}

class Ball extends Bitmap {
    constructor(x, y, width, height, path, opacity) {
        super(x, y, width, height, path, opacity);
        this.intendedX = x;
        this.intendedY = y;
        this.velocity = {x: 10, y: 0};
        this.bounced = false;
    }

    tick(ratioX, ratioY) {
        this.velocity.y += 1;
        this.intendedX += this.velocity.x;
        this.intendedY += this.velocity.y;
        this.x = ratioX*this.intendedX;
        this.y = ratioY*this.intendedY;
        //print(this.x, this.y);
    }

    bounce() {
        //print("bounce");
        this.bounced = true;
        this.velocity.y *= -.7;
        PlaySoundSpecial(__dirname+"/psgui/peghit_low.mp3", "peghit");
    }
}

class PeggleUI extends Parent { //i just spent the last few hours semi-rewritting mastercontrols so i could get hovering to work right with buttons (im ready to peg) <- when did i write that?
    backgroundimgs = [];
    loading = false; //loading is 0-25
    loadingLights = undefined;
    sunBmp = undefined;
    //sunFaceBmp = undefined;
    shadowBmp = undefined;
    peggleBmp = undefined;
    hookButton = undefined;
    peggleProcess = undefined;

    hwnd = undefined;
    intro = false;
    postIntroBall = undefined;
    timing = undefined;
    introLoadingTiming = undefined;
    bounceTiming = undefined;

    static loadingSlices = [
        {x: 0, y: 0, width: 18, height: 49},
        {x: 18, y: 0, width: 18, height: 49, kerningWidth: 12},
        {x: 36, y: 0, width: 18, height: 49, kerningWidth: 14}, //251 -> 13
        {x: 54, y: 0, width: 18, height: 49, kerningWidth: 13}, //238 -> 14
        {x: 72, y: 0, width: 18, height: 49, kerningWidth: 13, kerningHeight: 1}, //224
    ];

    static loadingIndices = [
        4, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0,
    ];

    startLoading() {
        this.hookButton.visible = false;
        this.loading = 1;
    }

    setLoading(v) {
        this.loading = v;
        //dirty = true;
        D2DWindow.singleton.paint();
    }

    stopLoading() {
        this.hookButton.visible = true;
        this.loading = 0;
    }

    hookErr(msg, addressToWriteDllPath, pathptr) {
        errMessage(msg);
        PlaySoundSpecial(__dirname+"/psgui/sigh.mp3", "sigh", this.hwnd);
        //if(snapshot) {
        //    CloseHandle(snapshot);
        //}
        if(addressToWriteDllPath) {
            VirtualFreeEx(this.peggleProcess, addressToWriteDllPath, 0, MEM_RELEASE);
        }
        if(pathptr) {
            DeleteArrayPtr(pathptr);
        }
        //if(thread) {
        //    CloseHandle(thread);
        //}
        //this.hookButton.visible = true;
        //this.setLoading(0);
        this.stopLoading();
    }

    hook() {
        let exitCode = undefined;
        this.startLoading();
        PlaySoundSpecial(`${__dirname}/psgui/timpaniroll.mp3`, "timpaniroll", this.hwnd);
        //lowkey just gonna find any window with peggle deluxe in it haha
        const titles = [];
        EnumWindows(function(hwnd) {
            const t = GetWindowText(hwnd);
            const {left, top} = GetWindowRect(hwnd);
            const client = GetClientRect(hwnd);
            if(t.includes("Peggle Deluxe") && client.right>0 && client.bottom>0) {
                titles.push(hwnd);
            }
            //else if(t.length){
            //    print(t, left, top);
            //}
        });
        if(titles.length) {
            //const peggle = hProcessFromHWND(titles[0]);
            //for(let i = 0; i < titles.length; i++) {
            //    const hwnd = titles[i];
            //    const t = GetWindowText(hwnd);
            //    print(`${i}: ${t}  -  ${hwnd}`);
            //}
            //const int = parseInt(getline("choose one FELLA: "));
            //
            this.peggleProcess = hProcessFromHWND(titles[0]);
            //print(titles[int], GetWindowText(titles[int]), this.peggleProcess);
            //nt create thread...          luckily no asm trickery this time i'll just pass the pointer to LoadLibraryA here!
            //but i wanted asm trickery :(
            //the trickery i was using so that x32dbg would load my custom dll wasn't too crazy (for some reason x64dbg doesn't seem to be able to use LoadLibrary on the Release build of an app?? idk kinda weirdly specific)
            //anyways all i did was set eip to a random patch of executable memory i found at exactly 0x00400289 (it's full of zeros at this spot because the pe header or something lol)
            //and wrote
            
            //push eax
            //push ebx
            //push ecx
            //push edx
            //push (addr+100) //i stored the path as a string literally right next to this asm in memory
            //call LoadLibraryA
            //int3
            //pop edx
            //pop ecx
            //pop ebx
            //pop eax
            //... path to dll
            
            //then i just set eip back to wherever i moved it from 
            //const kernel32 = LoadLibraryEx("kernel32.dll");
            //const LLA = GetProcAddress(kernel32, "LoadLibraryA");
            //print(kernel32, LLA);
            //ok apparently (according to chatgpt) the argument data is not copied and it just takes the pointer's word for it
            //this means i gotta allocate memory in the process i want to make the new thread in so i can call loadlibrary in there
            //that means we gotta use VirtualAlloc

            //yeah so i wrote it deeper in the if statements but since peggle is 32 bit, kernel32 is loaded somewhere else and I gotta find it
            //const pid = GetProcessId(this.peggleProcess); //uhh how i get pid from process
            //                                          for some reason i have to use both
            const LoadLibraryA = LocateAndGetProcAddressEx(this.peggleProcess, "kernel32.dll", "LoadLibraryA");
            if(!LoadLibraryA) {
                this.hookErr("LocateAndGetProcAddressEx failed! (maybe i wrote the wrong module or function name?)");
                return;
            }
            print("LLA:", LoadLibraryA);
            //return;
            const dllpath = "E:\\Users\\megal\\source\\repos\\PeggleScripting\\Release\\PeggleScripting.dll";
            const addressToWriteDllPath = VirtualAllocEx(this.peggleProcess, NULL, dllpath.length, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if(!addressToWriteDllPath) {
                this.hookErr("VirtualAllocEx failed! (perhaps you don't have permission? idk)");
                return;
            }

            this.setLoading(this.loading + 6);
            const pathptr = NewCharStrPtr(dllpath); //well since i "imported" marshallib i don't actually need this anymore
            print(addressToWriteDllPath, pathptr);
            //__debugbreak();
            //lowkey i was thinking about using marshallib's CString class but i can't be bothered so imma just allocate this string on the heap and delete it
            let success = WriteProcessMemory(this.peggleProcess, addressToWriteDllPath, pathptr, dllpath.length);
            print(success);
            if(!success) {
                this.hookErr("WriteProcessMemory failed!", addressToWriteDllPath, pathptr);
                return;
                //im making this weird nested if here instead of returning because if i return i'd skip out on freeing kernel32 (but more importantly i wouldn't reset the loading bar) (nevermind i solved that by making the hookErr method)
            }

            DeleteArrayPtr(pathptr);
            //__debugbreak();
            //yeah nevermind this ain't working it's time for plan b:
            //asm bullshit
            //WAIT WAIT
            //before we pull out any array buffers
            //i just realized that peggle is 32 bit while jbs is 64 bit
            //is my kernel32 in the same place as peggle's? (probably not right?) (ok yeah i just checked and FUCK no it's not the same)
            //ok we gotta do some BS to get the address of LoadLibraryA in the peggle process
            //const asm = new Uint8Array([
            //    0xcc,                                       //int3
            //    0xb8, ...int32_to_little_endian_hex(LLA),   //mov eax, imm32
            //    0xff, 0xd0,                                 //call eax
            //    0xc3,                                       //ret
            //]);
            ////alright apparently NtCreateThreadEx doesn't copy the function it's going to call either so
            ////we'll allocate some space for my function in the process (don't forget to make it executable) and we'll send a pointer to that
            //const addressForASM = VirtualAllocEx(this.peggleProcess, NULL, asm.byteLength, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            //print(kernel32, LLA, addressForASM);
            //__debugbreak();
            //if(!addressForASM) {
            //    const g = GetLastError();
            //    print("VirtualAllocEx failed for asm!\x07",g,_com_error(g));
            //}else {
    
                //this.loading += 6;
                //success = WriteProcessMemory(this.peggleProcess, addressForASM, asm);
                //if(!success) {
                //    const g = GetLastError();
                //    print("WriteProcessMemory (for asm) failed!\x07", g, _com_error(g));
                //}else {
                    this.setLoading(this.loading + 6);
                    // const thread = NtCreateThreadEx(THREAD_ALL_ACCESS, NULL, this.peggleProcess, addressForASM, addressToWriteDllPath, false, NULL, NULL, NULL); //hmm it's saying an invalid parameter was passed (yeah i wrote it wrong the first time)
                    const thread = NtCreateThreadEx(THREAD_ALL_ACCESS, NULL, this.peggleProcess, LoadLibraryA, addressToWriteDllPath, false, NULL, NULL, NULL);
                    print(thread);
                    if(!thread) {
                        this.hookErr("NtCreateThreadEx failed!", addressToWriteDllPath);
                        return;
                    }

                    this.setLoading(this.loading + 6);
                    const t = Date.now();
                    WaitForSingleObject(thread, 0xFFFFFFFF);
                    print("waited",Date.now()-t,"ms");
                    print("exit code",exitCode=GetExitCodeThread(thread));
                    CloseHandle(thread);
                    //VirtualFree
                    success = VirtualFreeEx(this.peggleProcess, addressToWriteDllPath, 0, MEM_RELEASE); //you can't pass a size with MEM_RELEASE
                    if(!success) {
                        this.hookErr("VirtualFreeEx failed but it's not that big of a deal!", addressToWriteDllPath); //lol
                    }
                    this.setLoading(this.loading + 6);
                    //success = VirtualFreeEx(this.peggleProcess, addressForASM, 0, MEM_RELEASE); //you can't pass a size with MEM_RELEASE
                    //if(!success) {
                    //    const g = GetLastError();
                    //    print("VirtualFreeEx failed for asm!\x07", g, _com_error(g));
                    //}
                    
                //}
            //}

            //CloseHandle(snapshot);
            //FreeLibrary(kernel32);
        }else {
            //print("Couldn't find peggle window?!\x07");
            this.hookErr("Couldn't find peggle window?!");
            return;
        }
        // PlaySoundSpecial(__dirname+"/psgui/odetojoy.mp3", "ode", this.hwnd);
        PlaySoundSpecial(__dirname+"/psgui/AwardFanfareV2_applause.mp3", "affv2_a", this.hwnd); //the applause is not included in the original AwardFanfareV2.ogg
        this.stopLoading();
        Msgbox("Success!", `it's probably working now (${exitCode})`, MB_OK | MB_SYSTEMMODAL);
    }

    //no lmao i just realized that i modify peggle directly and if i just unloaded it, peggle would crash right after that trying to execute random memory
    /*unhook() {
        const FreeLibrary = LocateAndGetProcAddressEx(this.peggleProcess, "kernel32.dll", "FreeLibrary");
        if(!FreeLibrary) {
            print("(unhook) LocateAndGetProcAddressEx failed!");
            return;
        }
        print("FL:", FreeLibrary);

        const dllpath = "E:\\Users\\megal\\source\\repos\\PeggleScripting\\Release\\PeggleScripting.dll";
        const addressToWriteDllPath = VirtualAllocEx(this.peggleProcess, NULL, dllpath.length, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if(!addressToWriteDllPath) {
            this.hookErr("VirtualAllocEx failed! (perhaps you don't have permission? idk)");
            return;
        }

        this.setLoading(this.loading + 6);
        const pathptr = NewCharStrPtr(dllpath);
        print(addressToWriteDllPath, pathptr);
        //__debugbreak();
        //lowkey i was thinking about using marshallib's CString class but i can't be bothered so imma just allocate this string on the heap and delete it
        let success = WriteProcessMemory(this.peggleProcess, addressToWriteDllPath, pathptr, dllpath.length);
        print(success);
        if(!success) {
            this.hookErr("WriteProcessMemory failed!", addressToWriteDllPath, pathptr);
            return;
            //im making this weird nested if here instead of returning because if i return i'd skip out on freeing kernel32 (but more importantly i wouldn't reset the loading bar)
        }

        DeleteArrayPtr(pathptr);

        const thread = NtCreateThreadEx(THREAD_ALL_ACCESS, NULL, this.peggleProcess, FreeLibrary, addressToWriteDllPath, false, NULL, NULL, NULL); //hmm it's saying an invalid parameter was passed
    
        this.setLoading(this.loading + 6);
        const t = Date.now();
        WaitForSingleObject(thread, 0xFFFFFFFF);
        print("waited",Date.now()-t,"ms");
        print("exit code",GetExitCodeThread(thread));
        CloseHandle(thread);
        //VirtualFree
        success = VirtualFreeEx(this.peggleProcess, addressToWriteDllPath, 0, MEM_RELEASE); //you can't pass a size with MEM_RELEASE
        if(!success) {
            this.hookErr("VirtualFreeEx failed but it's not that big of a deal!", addressToWriteDllPath); //lol
        }
        this.setLoading(this.loading + 6);

        this.stopLoading();
        Msgbox("Success!", "your peggle is probably normal again", MB_OK | MB_SYSTEMMODAL);
    }*/

    constructor(hwnd) {
        super(0, 0, w, h, [], [NULL]);
        this.hwnd = hwnd;
        this.sunBmp = new Bitmap(0, 0, 0, 0, `${__dirname}/psgui/SunGLow.png`);
        //this.sunFaceBmp = new Bitmap(0, 0, 0, 0, `${__dirname}/psgui/SunHead.png`, 0.0);
        this.shadowBmp = new Bitmap(0, 0, 0, 0, `${__dirname}/psgui/Shadow.png`, 0.0);
        this.peggleBmp = new Bitmap(0, 0, 0, 0, `${__dirname}/psgui/peggleloader.png`);
        this.backgroundimgs.push(
            //bitmaps initalized 0 here so i can do all the calculations in resize
            new Bitmap(0, 0, 0, 0, `${__dirname}/psgui/LoaderBack.png`),
            this.sunBmp,
            //this.sunFaceBmp,
            new Bitmap(0, 0, 0, 0, `${__dirname}/psgui/LoaderHill.png`),
            this.shadowBmp,
            //new Bitmap(0, 0, 0, 0, `${__dirname}/psgui/peggleloader.png`),
            this.peggleBmp,

            new Bitmap(0, 0, 0, 0, `${__dirname}/psgui/screenframeleft.png`),
            new Bitmap(0, 0, 0, 0, `${__dirname}/psgui/screenframetop.png`),
            new Bitmap(0, 0, 0, 0, `${__dirname}/psgui/screenframeright.png`),
            new Bitmap(0, 0, 0, 0, `${__dirname}/psgui/screenframebottom.png`),

        );
        this.loadingLights = d2d.CreateBitmapFromWicBitmap(wic.LoadBitmapFromFilename(`${__dirname}/psgui/Loaderlights.png`, wic.GUID_WICPixelFormat32bppPBGRA), true);
        const bitmap = new GenericTiledBitmap(0, 0, 0, 0, `${__dirname}/psgui/HookNOw.png`, 1.0, [{x: 0, y: 0, width: 344, height: 50}, {x: 0, y: 50, width: 344, height: 50}], 0);
        this.hookButton = new TiledBitmapButton(bitmap, this.hook, this);
        this.appendChild(this.hookButton);
        this.resize(w, h); //lol just so my bitmaps are scaled right
    }

    resize(newwidth, newheight) {
        this.width = newwidth;
        this.height = newheight;
        
        const ratioX = newwidth/800; //these images are meant to work with an 800 by 600 screen (based off their sizes lol)
        const ratioY = newheight/600;

        this.backgroundimgs[0].transform(0, 0, ratioX*800, ratioY*432);
        //this.backgroundimgs[1].transform(0, 0, 0, 0); //handled in the uhhh the uhhhh introLoop
        this.backgroundimgs[2].transform(0, ratioY*(600-316), ratioX*800, ratioY*316);
        //this.backgroundimgs[2].transform(ratioX*228, ratioY*445, ratioX*344, ratioY*50);
        this.hookButton.transform(ratioX*228, ratioY*445, ratioX*344, ratioY*50);
        //this.backgroundimgs[3].transform(ratioX*163, ratioY*286, ratioX*460, ratioY*236);
        this.shadowBmp.transform(ratioX*163, ratioY*286, ratioX*460, ratioY*236);
        //this.backgroundimgs[4].transform(ratioX*170, ratioY*139, ratioX*426, ratioY*212);
        this.peggleBmp.transform(ratioX*170, ratioY*139, ratioX*426, ratioY*212);
        
        this.backgroundimgs[5].transform(0, 0, ratioX*50, ratioY*600); //left
        this.backgroundimgs[6].transform(ratioX*50, 0, ratioX*700, ratioY*33); //top
        this.backgroundimgs[7].transform(ratioX*(800-50), 0, ratioX*50, ratioY*600); //right
        this.backgroundimgs[8].transform(ratioX*50, ratioY*(600-34), ratioX*700, ratioY*34); //bottom
    }

    redraw() {
        this.backgroundimgs.forEach(b=>b.redraw());
        this.hookButton.redraw();
        if(this.loading) {
            const ratioX = this.width/800;
            const ratioY = this.height/600;

            let almostx = ratioX*224;
            let y = ratioY*445;
            const w = ratioX*18;
            let h = y+(ratioY*49);

            for(let i = 0; i < this.loading; i++) {
                const index = PeggleUI.loadingIndices[i];
                const sourceRect = PeggleUI.loadingSlices[index];
                //print(almostx, w, almostx+w);
                //print(i, index, sourceRect);
                d2d.DrawBitmap(this.loadingLights, almostx, y, almostx+w, h, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, sourceRect.x, sourceRect.y, sourceRect.x+sourceRect.width, sourceRect.y+sourceRect.height);
                almostx += (ratioX*sourceRect.kerningWidth); //(w*i); //noooo the spacing is different sometimes...
                if(sourceRect.kerningHeight) {
                    y += sourceRect.kerningHeight*ratioY;
                    h += sourceRect.kerningHeight*ratioY;
                }
            }
        }
        if(this.postIntroBall) {
            this.postIntroBall.redraw();
        }
    }

    windowResized(oldw, oldh) {
        //idgaf about all that give me the new height
        this.resize(D2DWindow.singleton.width, D2DWindow.singleton.height);
    }

    beginIntro() {
        this.startLoading();
        this.timing = Date.now();
        this.intro = true;
        this.introLoadingTiming = Date.now();
        this.postIntroBall = new Ball(100, -100, 80, 80, `${__dirname}/psgui/LogoBall.png`, 1.0);
    }

    onTimer() {
        if(this.intro) {
            //wait i could lerp this lol
            if((Date.now()-this.introLoadingTiming) > 500) { //1.667
                this.introLoadingTiming = Date.now();
                this.setLoading(this.loading+1);
                if(this.loading == 25) {
                    this.intro = false;
                    this.stopLoading();
                    //this.sunFaceBmp.opacity = 1.0;
                }
            }
            const elapsed = (Date.now()-this.timing);

            const ratioX = this.width/800;
            const ratioY = this.height/600;

            const y = elapsed/13000;

            if(y < 1.0) {
                //print(ratioY*lerp(600, -300, y));

                // this.backgroundimgs[1].transform(-30, ratioY*lerp(600, -325, y), ratioX*800, ratioY*800);
                // this.backgroundimgs[4].opacity = y;
                this.sunBmp.transform(ratioX*-30, ratioY*lerp(400, -250, y), ratioX*800, ratioY*800);
                //this.sunFaceBmp.transform(ratioX*(-30+319), ratioY*lerp(600+308, -250+308, y), ratioX*150, ratioY*150);
            }

            if(elapsed > 6000) {
                const shadowOpacity = (elapsed-6000)/6000;
                this.shadowBmp.opacity = shadowOpacity;
            }
        }else {
            //spin the sun
            // print(GetKey(VK_RBUTTON), 1+(GetKey(VK_RBUTTON)), 1+(GetKey(VK_RBUTTON)>0));
            this.sunBmp.rotation -= 1+(GetKey(VK_RBUTTON)>0);
            //this.sunFaceBmp.rotation++;
            const ratioX = this.width/800;
            const ratioY = this.height/600;

            if(this.postIntroBall) {
                this.postIntroBall.tick(ratioX, ratioY);
                //                                                               +height here so that im checking if the bottom of the ball has hit (80 is the size of the ball)
                //print(this.postIntroBall.x, this.postIntroBall.y, this.peggleBmp.x, this.peggleBmp.y, withinBounds(this.peggleBmp, this.postIntroBall));
                //if(!this.postIntroBall.bounced && withinBounds({x: this.postIntroBall.x, y: this.postIntroBall.y+this.postIntroBall.height, width: this.postIntroBall.width, height: this.postIntroBall.height}, this.peggleBmp.x, this.peggleBmp.y)) {
                //haha i was using withinBounds backwarsds
                if(!this.postIntroBall.bounced && withinBounds(this.peggleBmp, {x: this.postIntroBall.x,y: this.postIntroBall.y+this.postIntroBall.height-50})) {
                    this.postIntroBall.bounce();
                    this.bounceTiming = Date.now();
                }
                if(this.postIntroBall.intendedY > 600+80) { //screen height + size of balle
                    this.postIntroBall.destroy();
                    this.postIntroBall = undefined;
                }
            }
            if(this.bounceTiming) {
                const elapsed = Date.now()-this.bounceTiming;
                //gonna make a down and up kinda motion here so
                //wait what the hell am i a lerp novice bruh
                //i could be using a transformation function on t right before lerping so i can get the max tuff effect
                //(refer to my magicquest.github.io/easing.html)

                //here's the professional way to do this:
                const t = elapsed/100;
                if(t > 50.0) { //since im using that SPECIAL godlikebounceease i have to go over 1 with t for it to do the residual bounces as seen in (https://www.desmos.com/calculator/jdklebdxwx)
                    this.bounceTiming = undefined;
                }else {
                    //function bounceEase(t) { //https://magicquest.github.io/easing.html?code=function%20ease(x)%20%7B%0A%20%20%20%20%2F%2F((1-x*2)**2)%3B%0A%20%20%20%20return%201-((1-x*2)**2)%0A%7D
                    //    //return 1-((1-t*2)**2); //just cooked this one up
                    //    //oh my god im throwing i could've just used sine!
                    //    return Math.sin(t*Math.PI);
                    //}
                    function godlikebounceease(t) { //oh shoot this stopped working because if t is 0 then this function returned infinity!
                        // const f = 2*(Math.ceil(t) || 1); //OR 1!
                        // return Math.abs(Math.sin(t*Math.PI)/f); //https://www.desmos.com/calculator/jdklebdxwx
                        
                        //more gradual
                        // const f = 0.5+((Math.ceil(t) || 1)/2);
                        // return Math.abs(Math.sin(t*Math.PI)/f); //https://www.desmos.com/calculator/uczu1crnjd
                        
                        //good enough
                        const f = (2*((Math.ceil(t) || 1)/4)**2)+1.875;
                        return Math.abs(Math.sin(t*Math.PI)/f); //https://www.desmos.com/calculator/lbsvrxuivb
                    }
                    const y = lerp(139, 145, godlikebounceease(t));
                    this.peggleBmp.y = ratioY*y;
                }

                //if(elapsed < 100) {
                //    //down
                //    const y = lerp(139, 145, elapsed/100);
                //    this.peggleBmp.y = ratioY*y;
                //}else if(elapsed < 200) {
                //    //up
                //    const y = lerp(145, 139, (elapsed-100)/100); //-100 so i can write it just like the first one (also because 100 ms have passed)
                //    this.peggleBmp.y = ratioY*y;
                //}else {
                //    this.bounceTiming = undefined;
                //}
            }
        }
        //print(dirty);
        dirty = true;
    }

    destroy() {
        super.destroy();
        this.backgroundimgs.forEach(b=>b.destroy());
        this.loadingLights.Release();
    }
}

let pui;

function windowProc(hwnd, msg, wp, lp) {
    if(msg == WM_CREATE) {
        wic = InitializeWIC(); ScopeGUIDs(wic);
        ({d2d, colorBrush, font, roundStrokeStyle} = D2DWindow.singleton.init(hwnd));
        // monofont = d2d.CreateFont("Consolas", 12);
        //print(d2d, colorBrush, font);
        pui = new PeggleUI(hwnd);
        print(panes, EditControl);
        panes.push(pui);
        //run intro
        pui.beginIntro();
        SetTimer(hwnd, 0, 16);
        PlaySoundSpecial(__dirname+"/psgui/Koka_morning_finaledit.mp3", "mangophonksigma", hwnd);
    }else if(msg == WM_TIMER) {
        pui.onTimer();
        //if(pui.loading == 25) {
        //    //KillTimer(hwnd, 21);
        //}
    }
    else if(msg == WM_DESTROY) {
        PostQuitMessage(0);
    }
}

const winclass = CreateWindowClass("psdllgui"/*, init*/, windowProc); //loop is not required y'all
winclass.hIcon = winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
//winclass.hbrBackground = COLOR_BACKGROUND;
winclass.hCursor = LoadCursor(NULL, IDC_ARROW);

void new D2DWindow(WS_EX_OVERLAPPEDWINDOW, winclass, "PeggleScripting GUI", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, w+20, h+43, undefined);
