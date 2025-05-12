from DashBoard.models import MemberInfo
from django.core.management.base import BaseCommand
import requests

class Member(BaseCommand):
    help = '국회의원 인적사항 API에서 데이터를 가져와 저장합니다.'

    def handle(self, *args, **kwargs):
        API_KEY = '0a630e0247e541f7895cf18260d41846'
        API_ID = 'nwvrqwxyaytdsfvhu'
        base_url = f'https://open.assembly.go.kr/portal/openapi/{API_ID}.do'

        page = 1
        total_saved = 0

        while True:
            params = {
                'KEY': API_KEY,
                'Type': 'json',
                'pIndex': page,
                'pSize': 100,
            }

            response = requests.get(base_url, params=params)
            result = response.json()

            # 오류 응답 처리
            if 'RESULT' in result.get(API_ID, [{}])[0]:
                result_code = result[API_ID][0]['RESULT']['CODE']
                if result_code != 'INFO-000':
                    print(f"❌ API 오류: {result[API_ID][0]['RESULT']['MESSAGE']}")
                    break

            # row 항목 추출
            data_list = result.get(API_ID, [])
            data = None
            for section in data_list:
                if 'row' in section:
                    data = section['row']
                    break

            if not data:
                break

            for item in data:
                if not item.get('HG_NM'):
                    continue

                Member.objects.update_or_create(
                    member_code=item.get('MONA_CD'),
                    defaults={
                        'name': item.get('HG_NM'),
                        'hanja_name': item.get('HJ_NM') or '',
                        'birth_date': item.get('BTH_DATE'),
                        'party': item.get('POLY_NM'),
                        'district': item.get('ORIG_NM'),
                        'elected_type': item.get('ELECT_GBN_NM'),
                        'reelection': item.get('REELE_GBN_NM'),
                        'gender': item.get('SEX_GBN_NM'),
                        'election_unit': item.get('UNITS'),
                        'phone': item.get('TEL_NO'),
                        'email': item.get('E_MAIL') or '',
                        'position': item.get('JOB_RES_NM'),
                        'main_committee': item.get('CMIT_NM') or '',

                    }
                )
                total_saved += 1

            print(f'{page}페이지 완료 - 누적 {total_saved}명 저장')
            page += 1

        self.stdout.write(self.style.SUCCESS(f'총 {total_saved}명의 국회의원 정보를 저장했습니다.'))
