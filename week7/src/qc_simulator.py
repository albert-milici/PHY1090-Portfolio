import numpy as np

workspace = np.array([[1.]])
namestack = []

def pushQubit(name,weights):
    global workspace
    global namestack
    if workspace.shape == (1,1):
        namestack = []
    namestack.append(name)
    weights = weights/np.linalg.norm(weights)
    weights = np.array(weights,dtype=workspace[0,0].dtype)
    workspace = np.reshape(workspace,(1,-1))
    workspace = np.kron(workspace,weights)

def tosQubit(name):
    global workspace
    global namestack
    k = len(namestack)-namestack.index(name)
    if k > 1:
        namestack.append(namestack.pop(-k))
        workspace = np.reshape(workspace,(-1,2,2**(k-1)))
        workspace = np.swapaxes(workspace,-2,-1)

def applyGate(gate,*names):
    global workspace
    for name in names:
        tosQubit(name)
    workspace = np.reshape(workspace,(-1,gate.shape[0]))
    np.matmul(workspace,gate.T,out=workspace)

def probQubit(name):
    global workspace
    tosQubit(name)
    workspace = np.reshape(workspace,(-1,2))
    prob = np.linalg.norm(workspace,axis=0)**2
    return prob/prob.sum()

def measureQubit(name):
    global workspace
    global namestack
    prob = probQubit(name)
    measurement = np.random.choice(2,p=prob)
    workspace = (workspace[:,[measurement]]/
                 np.sqrt(prob[measurement]))
    namestack.pop()
    return str(measurement)

def TOFF3_gate(q1,q2,q3,q4):
    pushQubit("temp",[1,0])
    applyGate(TOFF_gate,q1,q2,"temp")
    applyGate(TOFF_gate,"temp",q3,q4)
    applyGate(TOFF_gate,q1,q2,"temp")
    measureQubit("temp")

def TOFFn_gate(ctl,result):
    n = len(ctl)
    if n == 0:
        applyGate(X_gate,result)
    if n == 1:
        applyGate(CNOT_gate,ctl[0],result)
    elif n == 2:
        applyGate(TOFF_gate,ctl[0],ctl[1],result)
    elif n > 2:
        k=0
        while "temp"+str(k) in namestack:
            k=k+1
        temp = "temp"+str(k)
        pushQubit(temp,[1,0])
        applyGate(TOFF_gate,ctl[0],ctl[1],temp)
        ctl.append(temp)
        TOFFn_gate(ctl[2:],result)
        applyGate(TOFF_gate,ctl[0],ctl[1],temp)
        measureQubit(temp)


X_gate = np.array([[0, 1],
                   [1, 0]])

H_gate = np.array([[1, 1],
                   [1,-1]]) * np.sqrt(1/2)

T_gate = np.array([[1,                0],
                   [0,np.exp(np.pi/-4j)]])

Tinv_gate = np.array([[1, 0],
                      [0,np.exp(np.pi/4j)]])

CNOT_gate = np.array([[1, 0, 0, 0],
                      [0, 1, 0, 0],
                      [0, 0, 0, 1],
                      [0, 0, 1, 0]])

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
pushQubit("Q1",[1,1])
print(np.reshape(workspace,(1,-1)))
print(namestack)
pushQubit("Q2",[0,1])
print(np.reshape(workspace,(1,-1)))
print(namestack)

print(np.reshape(workspace,(1,-1)))
print(namestack)
tosQubit("Q1")
print(np.reshape(workspace,(1,-1)))
print(namestack)

print(np.reshape(workspace,(1,-1)))
print(namestack)
applyGate(H_gate,"Q2")
print(np.reshape(workspace,(1,-1)))
print(namestack)


workspace = np.array([[1.]])
pushQubit("Q1",[1,0])
applyGate(H_gate,"Q1")
print("Q1 probabilities:", probQubit("Q1"))
pushQubit("Q2",[0.6,0.8])
print("Q2 probabilities:", probQubit("Q2"))
print(measureQubit("Q1"), measureQubit("Q2"))


def toffEquiv_gate(q1,q2,q3):
    applyGate(H_gate,q3)
    applyGate(CNOT_gate,q2,q3)
    applyGate(Tinv_gate,q3)
    applyGate(CNOT_gate,q1,q3)
    applyGate(T_gate,q3)
    applyGate(CNOT_gate,q2,q3)
    applyGate(Tinv_gate,q3)
    applyGate(CNOT_gate,q1,q3)
    applyGate(T_gate,q2)
    applyGate(T_gate,q3)
    applyGate(H_gate,q3)
    applyGate(CNOT_gate,q1,q2)
    applyGate(T_gate,q1)
    applyGate(Tinv_gate,q2)
    applyGate(CNOT_gate,q1,q2)

workspace = np.array([[1.+0j]])
for i in range(16):
    pushQubit("Q1",[1,1])
    pushQubit("Q2",[1,1])
    pushQubit("Q3",[1,0])
    toffEquiv_gate("Q1","Q2","Q3")
    print(measureQubit("Q1")+measureQubit("Q2")+
          measureQubit("Q3"), end=",")
print()

workspace = np.array([[1.]])
for i in range(20):
    pushQubit("Q1",[1,1])
    pushQubit("Q2",[1,1])
    pushQubit("Q3",[1,1])
    pushQubit("Q4",[1,0])
    TOFF3_gate("Q1","Q2","Q3","Q4")
    print("".join([measureQubit(q) for q in
                   ["Q1","Q2","Q3","Q4"]]), end=",")
print()


workspace = np.array([[1]],dtype=np.single)
for i in range(20):
    pushQubit("Q1",[1,1])
    pushQubit("Q2",[1,1])
    pushQubit("Q3",[1,1])
    pushQubit("Q4",[1,0])
    TOFFn_gate(["Q1","Q2","Q3"],"Q4")
    print("".join([measureQubit(q) for q in
               ["Q1","Q2","Q3","Q4"]]),end=",")
print()