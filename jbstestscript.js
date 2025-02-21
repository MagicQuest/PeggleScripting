const ps = LoadLibraryEx("E:/Users/megal/source/repos/PeggleScripting/x64/Release/PeggleScripting.dll", NULL);
print(ps);
const ceg = GetProcAddress(ps, "ChooseExtendedGlobals");
const rle = GetProcAddress(ps, "RunLeCode");
let result = Call(rle, 4, [9, 10, 21, hInstance], [VAR_INT, VAR_INT, VAR_INT, VAR_INT], RETURN_NUMBER);
print("result+", result);

Call(ceg, 0, [], [], RETURN_VOID);

Call(rle, 4, [9, 10, 21, hInstance], [VAR_INT, VAR_INT, VAR_INT, VAR_INT], RETURN_NUMBER);
print("result+", result);