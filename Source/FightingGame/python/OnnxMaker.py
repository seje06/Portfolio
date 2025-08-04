import torch
import torch.onnx
from Dungeon_Final_A2C import ActorCritic

# 모델을 CPU로 로드하고 inference 모드로 설정
device = torch.device("cpu")
model = ActorCritic().to(device)
checkpoint = torch.load("../SavedModels/Dungeon_A2C/소소전투무/ckpt.pth", map_location=device)
model.load_state_dict(checkpoint["network"])
model.eval()  # inference 모드로 설정

# 모델을 입력 데이터에 맞게 설정
# ex) 입력 차원 1, 크기 5인 경우:(1,5)
dummy_input = torch.randn(1,3)
onnx_path = "C:/Users/sqlwh/OneDrive/문서/GitHub/Capstone_Unity-Network/Assets/OnnxModels/MoveTest2.onnx"
input_names = ["x"]  # 입력 노드의 이름 지정
output_names = ["y"]  # 출력 노드의 이름 지정
export_params=True
do_constant_folding=True
#Onnx파일 생성
torch.onnx.export(model, dummy_input,onnx_path,opset_version=10,input_names = ["x"],output_names = ["y"])
