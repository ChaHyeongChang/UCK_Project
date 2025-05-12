from DashBoard.models import MemberInfo
from django.core.management.base import BaseCommand
import requests
import re

class Command(BaseCommand):
    help = '국회의원 가결률 정보를 계산하여 DB에 저장합니다.'

    def handle(self, *args, **kwargs):
        API_KEY = '0a630e0247e541f7895cf18260d41846'
        API_ID  = 'nzmimeepazxkubdpn'
        base_url = f'https://open.assembly.go.kr/portal/openapi/{API_ID}.do'

        page = 1
        proposer_stats = {}

        def clean_name(name):
            return re.sub(r'\s*\(.*?\)', '', name).strip()

        # 1. 페이징 루프
        while True:
            print(f"📄 현재 {page}페이지 요청 중...")
            params = {
                'KEY': API_KEY,
                'Type': 'json',
                'pIndex': page,
                'pSize': 100,
                'AGE': '22',
            }
            resp = requests.get(base_url, params=params, timeout=20)
            print(f"응답 코드: {resp.status_code}, 데이터 길이: {len(resp.text)}")

            result    = resp.json()
            data_list = result.get(API_ID, [])
            data      = None
            for sec in data_list:
                if 'row' in sec:
                    data = sec['row']
                    break

            if not data:
                print("🚫 더 이상 데이터 없음, 수집 종료")
                break

            print(f"📦 가져온 데이터 수: {len(data)}")

            # 2. 한 페이지 내 각 법안 처리
            for item in data:
                # ──── 결과 문자열 확보
                result_str = str(item.get('PROC_RESULT') or '').strip()
                # 심의 전(null) 법안은 건수에도 포함하지 않음
                if not result_str:
                    continue

                # ──── 발의자 이름 추출
                main = item.get('RST_PROPOSER', '')
                co   = item.get('PUBL_PROPOSER', '')
                names = set()
                if main:
                    names.add(clean_name(main))
                if co and isinstance(co, str):
                    for nm in co.split(','):
                        names.add(clean_name(nm))

                # ──── 통계 집계
                for nm in names:
                    if not nm:
                        continue
                    if nm not in proposer_stats:
                        proposer_stats[nm] = {'total': 0, 'passed': 0}
                    proposer_stats[nm]['total'] += 1
                    if result_str in ('가결', '원안가결', '수정가결'):
                        proposer_stats[nm]['passed'] += 1

            print(f"✅ {page}페이지 처리 완료")
            page += 1

        # 3. DB에 저장
        for member in MemberInfo.objects.all():
            key = clean_name(member.name)
            stat = proposer_stats.get(key)
            if stat:
                total  = stat['total']
                passed = stat['passed']
                rate   = round(passed / total * 100, 2) if total else 0
                member.total_bills  = total
                member.passed_bills = passed
                member.pass_rate    = rate
                member.save()
                print(f"🟢 {member.name}: {total}건 → {passed}건 → 가결률 {rate}%")
            else:
                print(f"⚪ {member.name}: 처리된 법안 없음")

        self.stdout.write(self.style.SUCCESS("국회의원별 가결률 계산 완료"))
