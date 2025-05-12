from DashBoard.models import MemberInfo as Member
from django.core.management.base import BaseCommand
import requests

class Command(BaseCommand):
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

            members_to_create = []

            for item in data:
                if not item.get('HG_NM') or not item.get('MONA_CD'):
                    continue

                member = Member(
                    member_code=item.get('MONA_CD'),
                    name=item.get('HG_NM'),
                    hanja_name=item.get('HJ_NM') or '',
                    birth_date=item.get('BTH_DATE'),
                    party=item.get('POLY_NM') or '',
                    district=item.get('ORIG_NM') or '',
                    elected_type=item.get('ELECT_GBN_NM') or '',
                    reelection=item.get('REELE_GBN_NM') or '',
                    gender=item.get('SEX_GBN_NM') or '',
                    election_unit=item.get('UNITS') or '',
                    phone=item.get('TEL_NO') or '',
                    email=item.get('E_MAIL') or '',
                    position=item.get('JOB_RES_NM') or '',
                    main_committee=item.get('CMIT_NM') or '',
                )

                members_to_create.append(member)

            Member.objects.bulk_create(members_to_create, ignore_conflicts=True)
            total_saved += len(members_to_create)

            print(f'{page}페이지 완료 - 누적 {total_saved}명 저장')
            page += 1

        self.stdout.write(self.style.SUCCESS(f'총 {total_saved}명의 국회의원 정보를 저장했습니다.'))
