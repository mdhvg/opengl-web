import "./App.css";
import fragShader from "../src-cpp/programs/noise/rsc/frag.txt?raw";
import vertShader from "../src-cpp/programs/noise/rsc/vert.txt?raw";
import { useEffect, useRef } from "react";
import { MainModule } from "./noise";
// @ts-ignore
import Wasm from "./noise.mjs";

function App() {
  const canvasRef = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    const Module = {
      canvas: canvasRef.current,
    };
    let wasm;
    Wasm(Module).then((w: MainModule) => {
      wasm = w;
      console.log(wasm);
      const vertPtr = wasm.stringToNewUTF8(vertShader);
      const fragPtr = wasm.stringToNewUTF8(fragShader);

      const args = [vertPtr, fragPtr];
      const argvPtr = wasm._malloc(args.length * Uint32Array.BYTES_PER_ELEMENT);

      for (let i = 0; i < args.length; i++) {
        wasm.HEAP32[argvPtr / 4 + i] = args[i];
      }

      wasm._init(2, argvPtr);
      wasm._free(vertPtr);
      wasm._free(fragPtr);
    });
  }, []);

  return (
    <canvas id={"canvas"} ref={canvasRef} width={900} height={900}></canvas>
  );
}

export default App;
