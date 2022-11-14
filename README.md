Vectorized Matrix Multiplier Vitis HLS IP (Half-Precision Fixed-Point)

Why is this important?:
 - Typically only small matrices can be processed on PL due to small BRAM sizes... so with this project you're limited by how much DRAM is available. With decent sizes of DRAM (even 512 MB), you can do matrix multiplications on very large matrices.
 - This design is as fast as numpy (if not faster), is small enough to fit on a lot of FPGA devices, uses a low amount of power, and can be used in a lot of ways.
 - IP can run in parallel with several of the same IP to generate results for matrix multiplication of very large matrices in a short time
 - IP is system-agnostic just needs Master AXI controls & data connections, and Slave AXI Lite controls & data connections.

Results from Running the Matrix Multiplication IP on the PYNQ-Z1 and Kria KV260:
![alt text](https://github.com/nick-nuti/Pynq-m_axi-matrix-multiplier/blob/main/results.PNG)

Notice:
- Resultant timing is from multiplying two of the same size matrices together [ex: 2000x2000 is specified in the picture, so matrix one (2000x2000) is multiplied by matrix two (2000x2000)]
- IP on Kria KV260 was run at 300 MHz
- IP on PYNQ-Z1 was run at 180 MHz

Validation Testbench:
- Testbench runs the IP on each board with a Zynq processor and generates inputs and outputs using the PYNQ interface
- Data is normalized in half-precision fixed-point format, sent to the IP, and is recieved and must be denormalized (example of this in the jupyter notebook)
- PYNQ (jupyter notebook) is responsible for generating the matrices, sending the matrices to the IP, and comparing results against numpy
- Vivado project connects the Zynq to the Vitis HLS IP
- Vitis HLS IP completes the matrix multiplication and sends the results back

How to run this project?
- let's say we're doing this for the KRIA KV260....
- Open Vitis HLS 2022.1
- Take the two github files from the "Vitis HLS" directory and import them to a Vitis HLS project specifically for part "xck26-sfvc784-2LV-c"
- Solution -> Run C Synthesis, change period to "3.33", and press "ok"
- Solution -> Export RTL

- Open Vivado 2022.1.2
- Create new project for Kria KV260
- Left pane -> Settings -> IP -> Repository -> "+" -> navigate to the Vitis HLS project directory and hit "ok"
- Use the TCL Console, navigate to the TCL script inside of the github directory "Vivado/2022.1.2", and the block diagram will generate
- Sources -> Design Sources -> Right-Click the block diagram design source -> "Create HDL Wrapper" -> "Let Vivado manage Wrapper and auto-update"
- Generate Bitstream
- When complete, go to project directory, search for .bit and .hwh files and copy them somewhere else, change names to matrixmult.hwh and matrixmult.bit for PYNQ

- PYNQ
- Copy the jupyter notebook file from github directory "PYNQ - Kriakv260/mm.ipynb" to FPGA jupyter notebook
- Copy matrixmult.bit and matrixmult.hwh to FPGA jupyter notebook
- Run
