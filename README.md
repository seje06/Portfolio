# 게임 개발 포트폴리오

---

## 목차
- [CPP Multi Game](#1-cpp-multi-game)
- [크레이지닷지볼 - Unreal](#2-크레이지닷지볼-crazy-dodgeball)
- [비스트 오브 아레나 - Unity](#3-비스트-오브-아레나)

---
---

## 1. CPP Multi Game
<table>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/57325196-ae8e-4b51-8b3f-60f349aaa87a" width="500"/></td>
    <td><img src="https://github.com/user-attachments/assets/eb5cb4bc-f506-4d4b-84de-3105676c0c9a" width="500"/></td>
  </tr>
</table>
<table>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/0868c1eb-6812-4fdc-8bd8-c4159d88253d" width="500"/></td>
    <td><img src="https://github.com/user-attachments/assets/0f7bbaa9-6e27-425f-a6dc-53972e108b01" width="500"/></td>
  </tr>
</table>

[멀티 게임 구현서(PDF)](https://github.com/user-attachments/files/22655387/Cpp.pdf), 
[소스코드](https://github.com/seje06/CPP_NetGame)
---

### 기본 정보
- **장르**: 웨이브 기반 2D 멀티게임
- **사용 툴 및 언어**: VS, window api, c++
- **형상 관리**: 깃허브 데스크탑 
- **플랫폼**: PC (Windows)  
- **개발 인원**: 프로그래머 1명  
- **개발 기간**: 2025년 8 ~ 9월

---

### 플레이 영상
해당 영상은 DHCP예약, 포트포워딩 과정을 거쳐 실제 외부 피시와 통신하여 진행한 게임 영상입니다.
- (https://www.youtube.com/shorts/1pEHYDXp5FY)

---

### 기능 구현
- 중력 처리
- 플레이어 및 AI
- 적 웨이브 시스템
- udp 소켓 기반의 신뢰성 구현  
- 멀티스레드를 통해 게임과 패킷처리 분리  
- mutex를 활용하여 데이터 경쟁 해결
  
---
---

## 2. 크레이지닷지볼 (Crazy Dodgeball)

<table>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/1e15ea8a-2442-42dd-8835-4a561bf9a018" width="500"/></td>
    <td><img src="https://github.com/user-attachments/assets/028c41ef-98d4-4617-96f8-000c949cd150" width="500"/></td>
  </tr>
</table>

[자세한 소개(PDF)](https://github.com/user-attachments/files/20431727/_.pdf), 
[핵심 기능 기술서.pdf](https://github.com/user-attachments/files/20967836/default.pdf)

[소스코드](https://github.com/seje06/Portfolio/tree/main/Source/CreazyDodgeball)
---

### 기본 정보
- **장르**: 1인칭 시점, 스킬을 활용한 캐쥬얼 피구 게임, PvE  
- **사용 툴 및 언어**: 언리얼 5.44, c++
- **형상 관리**: 깃허브 데스크탑 
- **플랫폼**: PC (Windows)  
- **개발 인원**: 프로그래머 2명  
- **개발 기간**: 2025년 4월 1일 ~ 5월 6일

---

### 플레이 영상
- (https://youtu.be/Lq0M5MXvgk4)

---

### 기능 구현
코드 비중 약 70%
- 공 제어 시스템
- 플레이하는 캐릭터 전환 
- 스킬 시스템 구현 (볼 거대화, Fire볼, 부메랑볼, 분신볼)  
- Dodgeball 궤적 및 충돌 처리  
- AI 캐릭터 상태 기반 행동 구현  
- 게임 UI: 체력, 스킬 쿨타임, 게임 오버 처리  
- 시네마틱을 활용한 인트로
  


---
---

## 3. 비스트 오브 아레나

해당 프로젝트는 대학교 전공 캡스톤 과제로, 이러한 시도를 해봤다는것에 의의를 두고 있습니다.

<table>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/2e6c2090-9c91-4586-b378-e54000f0981c" width="500"/></td>
    <td><img src="https://github.com/user-attachments/assets/326573f1-2999-4b8a-bd34-6f279d60e860" width="500"/></td>
  </tr>
</table>

[논문(PDF)](https://github.com/user-attachments/files/20435744/_.-.pdf)

[소스코드](https://github.com/seje06/Portfolio/tree/main/Source/FightingGame)
---

### 기본 정보

- **장르**: 3인칭 쿼터뷰 시점, 머신러닝을 활용한 멀티대전게임, PvP, PvE
- **사용 툴 및 언어**: 유니티, C#
- **형상 관리**: 깃허브 데스크탑 
- **플랫폼**: PC (Windows)  
- **개발 인원**: 프로그래머 1명 외 3명
- **개발 기간**: 약2개월

---

### 플레이 영상
- (추후 추가 예정)

---

### 기능 구현
- 기본 동작(이동, 공격, 구르기 등)
- 포톤 Pun2을 활용한 멀티 구현(로비, 룸, 채팅 등)
- 스킬 시스템 구현 (공격, 방어, 유틸)
- 유니티 MLAgent, 파이선을 활용한 AI구현

