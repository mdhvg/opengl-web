import "./App.css";
import fragShader from "../src-cpp/programs/noise/rsc/frag.txt?raw";
import vertShader from "../src-cpp/programs/noise/rsc/vert.txt?raw";
import Wasm from "./noise.js";
import { useRef } from "react";

function App() {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  Wasm().then((wasm) => {
    console.log(wasm);

    const vertPtr = wasm.stringToNewUTF8(vertShader);
    const fragPtr = wasm.stringToNewUTF8(fragShader);

    const args = [vertPtr, fragPtr];
    const argvPtr = wasm._malloc(args.length * Uint32Array.BYTES_PER_ELEMENT);

    for (let i = 0; i < args.length; i++) {
      wasm.HEAP32[argvPtr / 4 + i] = args[i];
    }

    wasm.canvas = canvasRef.current!;

    wasm._main(2, argvPtr);
    wasm._free(vertPtr);
    wasm._free(fragPtr);
  });
  return (
    <canvas
      ref={canvasRef}
      id="canvas"
      tabIndex={-1}
      autoFocus
      width={950}
      height={950}
    ></canvas>
  );
}

export default App;
