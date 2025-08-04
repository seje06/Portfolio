import numpy as np
import collections
import datetime
import platform
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.distributions import Categorical
from collections import deque
from mlagents_envs.environment import UnityEnvironment, ActionTuple
from mlagents_envs.side_channel.engine_configuration_channel import EngineConfigurationChannel
import mlagents_envs
import os
import json

# 하이퍼파라미터 설정
learning_rate = 0.001
gamma = 0.95
action_size = 19
n_rollout=1000
print_interval = 10
save_interval = 20
buffer_limit=100000

load_model = True
train_mode =True
load_buffer=False
save_buffer=False

BufferFile_Route="C:/Unity/ML/Capstone_Project/Buffers/Dungeon_Buffer.json"

game = "ML_Dungeon"
os_name = platform.system()
env_name = f"C:/Unity/ML/Capstone_Project/BuildedGames/Dungeon_Final/{game}.exe"
if os_name == 'Windows':
    env_name = f"C:/Unity/ML/Capstone_Project/BuildedGames/Dungeon_Final/{game}.exe"
elif os_name == 'Darwin':
    env_name = f"../envs/{game}_{os_name}"

date_time = datetime.datetime.now().strftime("%Y%m%d%H%M%S")
save_path = f"C:/Unity/ML/Capstone_Project/SavedModels/Dungeon_A2C/{date_time}"
load_path = f"C:/Unity/ML/Capstone_Project/SavedModels/Dungeon_A2C/20241106143003"
if not load_model:
    os.makedirs(save_path, exist_ok=True)
else:
    os.makedirs(save_path, exist_ok=True)
    #save_path = load_path

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(torch.__version__)

class ActorCritic(nn.Module):
    def __init__(self):
        super(ActorCritic, self).__init__()
        self.fc1 = nn.Linear(3, 256)
        #self.bn1 = nn.BatchNorm1d(256)
        self.fc2 = nn.Linear(256, 256)
        #self.bn2 = nn.BatchNorm1d(256)
        self.fc3 = nn.Linear(256,256)
        #self.dropout = nn.Dropout(p=0.5)
        #self.dropout1 = nn.Dropout(p=0.2)  

        self.fc_policy = nn.Linear(256, action_size)  # 정책 출력 (Policy)
        self.fc_value = nn.Linear(256, 1)    # 가치 출력 (Value)

    def forward(self,x):
        #x = self.dropout1(F.leaky_relu(self.fc1(x)))
        #x = self.dropout2(F.leaky_relu(self.fc2(x)))
        #x = self.dropout3(F.leaky_relu(self.fc3(x)))
        #x = self.dropout4(F.leaky_relu(self.fc4(x)))
        #x = self.dropout5(F.leaky_relu(self.fc5(x)))
        x=F.leaky_relu(self.fc1(x))
        #x=self.dropout(x)
        x=F.leaky_relu(self.fc2(x))
        #x=self.dropout1(x)
        x=F.leaky_relu(self.fc3(x))
        #x = F.leaky_relu(self.fc4(x))
        #x=F.leaky_relu(self.fc5(x))
        x = self.fc_policy(x)
        return x
    
    def pi(self, x, softmax_dim=-1):
        #x = self.dropout1(F.leaky_relu(self.fc1(x)))
        #x = self.dropout2(F.leaky_relu(self.fc2(x)))
        #x = self.dropout3(F.leaky_relu(self.fc3(x)))
        #x = self.dropout4(F.leaky_relu(self.fc4(x)))
        #x = self.dropout5(F.leaky_relu(self.fc5(x)))
        x=F.leaky_relu(self.fc1(x))
        #x=self.dropout(x)
        x=F.leaky_relu(self.fc2(x))
        #x=self.dropout1(x)
        x=F.leaky_relu(self.fc3(x))
        #x = F.leaky_relu(self.fc4(x))
        #x=F.leaky_relu(self.fc5(x))
        x = self.fc_policy(x)
        prob = F.softmax(x, dim=softmax_dim)
        return prob

    def v(self, x):
        #x = self.dropout1(F.leaky_relu(self.fc1(x)))
        #x = self.dropout2(F.leaky_relu(self.fc2(x)))
        #x = self.dropout3(F.leaky_relu(self.fc3(x)))
        #x = self.dropout4(F.leaky_relu(self.fc4(x)))
        #x = self.dropout5(F.leaky_relu(self.fc5(x)))
        x=F.leaky_relu(self.fc1(x))
        #x=self.dropout(x)
        x=F.leaky_relu(self.fc2(x))
        #x=self.dropout1(x)
        x=F.leaky_relu(self.fc3(x))
        #x = F.leaky_relu(self.fc4(x))
        #x=F.leaky_relu(self.fc5(x))
        v = self.fc_value(x)
        return v

class ActorCriticAgent:
    def __init__(self):
        self.network = ActorCritic().to(device)
        self.optimizer = optim.Adam(self.network.parameters(), lr=learning_rate)
        self.scheduler = optim.lr_scheduler.StepLR(self.optimizer, step_size=100, gamma=0.9)  # 학습률 스케줄러 추가
        self.data = []
        self.buffer=[]
        self.data_set=set()
        self.delivery=0

        if load_model:
            print(f"...Load Model from {load_path}/ckpt ...")
            checkpoint = torch.load(load_path + '/ckpt.pth', map_location=device)
            self.network.load_state_dict(checkpoint["network"])
            self.optimizer.load_state_dict(checkpoint["optimizer"])

    def put_data(self, transition):
        self.data.append(transition)

    def make_batch(self):
        s_lst, a_lst, r_lst, s_prime_lst, done_mask_lst = [], [], [], [], []
        for transition in reversed(self.data):
            s, a, r, s_prime, done = transition
            s_lst.append(s)
            a_lst.append([a])
            r_lst.append(r)
            s_prime_lst.append(s_prime)
            done_mask = 0 if done else 1
            done_mask_lst.append([done_mask])

        # 리스트를 텐서로 변환시 속도가 좋지않아 넘파이 배열로 변환
        s_arr = np.array(s_lst, dtype=np.float32)
        a_arr = np.array(a_lst, dtype=np.int64)
        r_arr = np.array(r_lst)
        s_prime_arr = np.array(s_prime_lst, dtype=np.float32)
        done_mask_arr = np.array(done_mask_lst)

        s_batch = torch.tensor(s_arr, dtype=torch.float).to(device)
        a_batch = torch.tensor(a_arr, dtype=torch.long).to(device)
        r_batch = torch.tensor(r_arr, dtype=torch.float).to(device)
        s_prime_batch = torch.tensor(s_prime_arr, dtype=torch.float).to(device)
        done_batch = torch.tensor(done_mask_arr, dtype=torch.float).to(device)
        self.data = []
        self.data_set.clear()
        torch.cuda.empty_cache() 
        return s_batch, a_batch, r_batch, s_prime_batch, done_batch

    def train_net(self):
        s, a, r, s_prime, done = self.make_batch()

        td_target = r + gamma * self.network.v(s_prime) * done
        delta = td_target - self.network.v(s)

        pi = self.network.pi(s, softmax_dim=-1)
        pi_a = pi.gather(1, a)
        loss = -torch.log(pi_a) * delta.detach() + F.smooth_l1_loss(self.network.v(s), td_target.detach())

        self.optimizer.zero_grad()
        loss.mean().backward()
        torch.nn.utils.clip_grad_norm_(self.network.parameters(), max_norm=0.5)  # Gradient Clipping 추가
        self.optimizer.step()
        #self.scheduler.step()

    def save_model(self):
        print("save model")
        torch.save({
            "network": self.network.state_dict(),
            "optimizer": self.optimizer.state_dict()
        }, save_path + "/ckpt.pth")

def main():
    engine_configuration_channel = EngineConfigurationChannel()
    env = UnityEnvironment(file_name=env_name, timeout_wait=20, side_channels=[engine_configuration_channel])
    env.reset()
    behavior_name = list(env.behavior_specs.keys())[0]
    spec = env.behavior_specs[behavior_name]
    env.reset()
    if train_mode:
        engine_configuration_channel.set_configuration_parameters(time_scale=20)
    else:
        engine_configuration_channel.set_configuration_parameters(time_scale=1)
    dec, term = env.get_steps(behavior_name)
    agent = ActorCriticAgent()
    score = 0.0
    if load_buffer and train_mode:
        with open(BufferFile_Route,'r') as f:
            agent.buffer=collections.deque(json.load(f),maxlen=buffer_limit)
    for n_epi in range(30000):
        done = False
        while agent.data_set.__len__()<n_rollout or done is False:
            s = dec.obs[0]
            s_list=s.squeeze().tolist()
            s = torch.tensor(s, dtype=torch.float).to(device).squeeze()
            s = np.array(s.cpu())
            prob = agent.network.pi(torch.from_numpy(s).float().to(device))
            m = Categorical(prob)
            a = m.sample().item()
            s_list.append(a)
            a_tuple = ActionTuple()
            a_tuple.add_discrete(np.array([[a]]))
            env.set_actions(behavior_name, a_tuple)
            env.step()
            dec, term = env.get_steps(behavior_name)
            done = len(term.agent_id) > 0
            r = term.reward if done else dec.reward
            s_list.append(r.squeeze().tolist())
            partial_training_data=tuple(s_list)
            s_prime = term.obs[0] if done else dec.obs[0]
            s_prime = torch.tensor(s_prime, dtype=torch.float).to(device).squeeze()
            s_prime = np.array(s_prime.cpu())

            if train_mode:
                if r.squeeze()<-30:
                    continue
                traning_data=(s, a, r, s_prime, done)
                if partial_training_data in agent.data_set:
                    agent.data_set.add(partial_training_data)
                    agent.put_data(traning_data)
                    score += r
                else:
                    agent.data_set.add(partial_training_data)
                    agent.put_data(traning_data)
                    score += r
            else:
                if r.squeeze()<-50:
                    continue
                agent.data_set.add(partial_training_data)
                score += r
        if train_mode:
            if n_epi % 1 == 0 :
                agent.train_net()
                agent.save_model()
                print(f"# of episode :{n_epi}, avg score : {score.squeeze()/print_interval:.1f}")
                score = 0.0
        else:
            print(f"# of episode :{n_epi}, avg score : {score.squeeze()/print_interval:.1f}")
            agent.data_set.clear()
            score = 0.0    
    env.close()

if __name__ == '__main__':
    main()