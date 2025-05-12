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

        # 1. 법안 데이터 수집
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
            result = resp.json()
            data_list = result.get(API_ID, [])
            data = None
            for sec in data_list:
                if 'row' in sec:
                    data = sec['row']
                    break

            if not data:
                print("🚫 더 이상 데이터 없음, 수집 종료")
                break

            for item in data:
                result_str = str(item.get('PROC_RESULT') or '').strip()
                if not result_str:
                    continue

                main = item.get('RST_PROPOSER', '')
                co   = item.get('PUBL_PROPOSER', '')
                names = set()
                if main:
                    names.add(clean_name(main))
                if co and isinstance(co, str):
                    for nm in co.split(','):
                        names.add(clean_name(nm))

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

        # 2. 의원 정보에 반영 (성능 개선)
        to_update = []

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
                to_update.append(member)
                print(f"🟢 {member.name}: {total}건 → {passed}건 → {rate}%")
            else:
                print(f"⚪ {member.name}: 처리된 법안 없음")

        # 3. DB에 일괄 저장
        MemberInfo.objects.bulk_update(to_update, ['total_bills', 'passed_bills', 'pass_rate'])

        self.stdout.write(self.style.SUCCESS("국회의원별 가결률 계산 완료"))
