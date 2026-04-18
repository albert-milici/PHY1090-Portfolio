import numpy as np

workspace = np.array([[1.]])

def pushQubit(weights):
    global workspace
    workspace = np.reshape(workspace,(1,-1))
    workspace = np.kron(workspace,weights)

def applyGate(gate):
    global workspace
    workspace = np.reshape(workspace,(-1,gate.shape[0]))
    np.matmul(workspace,gate.T,out=workspace)

def tosQubit(k):
    global workspace
    if k > 1:
        workspace = np.reshape(workspace,(-1,2,2**(k-1)))
        workspace = np.swapaxes(workspace,-2,-1)

def probQubit():
    global workspace
    workspace = np.reshape(workspace,(-1,2))
    return np.linalg.norm(workspace,axis=0)**2

def measureQubit():
    global workspace
    prob = probQubit()
    measurement = np.random.choice(2,p=prob)
    workspace = (workspace[:,[measurement]]/
    np.sqrt(prob[measurement]))
    return str(measurement)

X_gate = np.array([[0, 1],
                   [1, 0]])

Y_gate = np.array([[ 0,-1j],
                   [1j,  0]])

Z_gate = np.array([[1, 0],
                   [0,-1]])

H_gate = np.array([[1, 1],
                   [1,-1]]) * np.sqrt(1/2)

S_gate = np.array([[1, 0],
                   [0,1j]])

T_gate = np.array([[1,                0],
                   [0,np.exp(np.pi/-4j)]])

Tinv_gate = np.array([[1, 0],
                      [0,np.exp(np.pi/4j)]])

def P_gate(phi):
    return np.array([[1,             0],
                     [0,np.exp(phi*1j)]])

def Rx_gate(theta):
    return np.array([[np.cos(theta/2),-1j*np.sin(theta/2)],
                     [-1j*np.sin(theta/2),np.cos(theta/2)]])

def Ry_gate(theta):
    return np.array([[np.cos(theta/2),-np.sin(theta/2)],
                     [np.sin(theta/2), np.cos(theta/2)]])

def Rz_gate(theta):
    return np.array([[np.exp(-1j*theta/2),                0],
                     [                  0,np.exp(1j*theta/2)]])

CNOT_gate = np.array([[1, 0, 0, 0],
                      [0, 1, 0, 0],
                      [0, 0, 0, 1],
                      [0, 0, 1, 0]])

CZ_gate = np.array([[1, 0, 0, 0],
                    [0, 1, 0, 0],
                    [0, 0, 1, 0],
                    [0, 0, 0,-1]])

SWAP_gate = np.array([[1, 0, 0, 0],
                      [0, 0, 1, 0],
                      [0, 1, 0, 0],
                      [0, 0, 0, 1]])

TOFF_gate = np.array([[1, 0, 0, 0, 0, 0, 0, 0],
                      [0, 1, 0, 0, 0, 0, 0, 0],
                      [0, 0, 1, 0, 0, 0, 0, 0],
                      [0, 0, 0, 1, 0, 0, 0, 0],
                      [0, 0, 0, 0, 1, 0, 0, 0],
                      [0, 0, 0, 0, 0, 1, 0, 0],
                      [0, 0, 0, 0, 0, 0, 0, 1],
                      [0, 0, 0, 0, 0, 0, 1, 0]])

# tests

workspace = np.array([[1.]])
pushQubit([1,0])
print("input", workspace)
applyGate(X_gate)
print("NOT output", workspace)

workspace = np.array([[1.]])
pushQubit([1,0])
print("input", workspace)
applyGate(H_gate)
print("H output", workspace)

workspace = np.array([[1.]])
pushQubit([1,0])
pushQubit([0.6,0.8])
print("before swap", workspace)
applyGate(SWAP_gate)
print("after swap", workspace)

workspace = np.array([[1.]])
pushQubit([1,0])
pushQubit([0.6,0.8])
print("before tos", workspace)
tosQubit(2)
print("after tos", np.reshape(workspace,(1,-1)))

workspace = np.array([[1.]])
for n in range(30):
    pushQubit([0.6,0.8])
    print(measureQubit(), end="")
print()

workspace = np.array([[1.]])
for i in range(16):
    pushQubit([1,0])
    applyGate(H_gate)
    pushQubit([1,0])
    applyGate(H_gate)
    pushQubit([1,0])
    applyGate(TOFF_gate)
    q3 = measureQubit()
    q2 = measureQubit()
    q1 = measureQubit()
    print(q1+q2+q3,end=",")
print()