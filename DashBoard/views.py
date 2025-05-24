import locale
from django.core.paginator import Paginator
from django.shortcuts import render, get_object_or_404
from .models import MemberInfo, MemberImageInfo

def home_view(request):
    return render(request, 'home.html')

def about_view(request):
    return render(request, 'about.html')

def rate_view(request):
    query = request.GET.get('q', '')
    if query:
        members = list(MemberInfo.objects.filter(
            elected_type='비례대표',
            name__icontains=query
        ))
    else:
        members = list(MemberInfo.objects.filter(
            elected_type='비례대표'
        ))

    # locale 설정 (리눅스/맥: 'ko_KR.utf8', 윈도우: 'Korean_Korea.949')
    try:
        locale.setlocale(locale.LC_COLLATE, 'ko_KR.utf8')
    except locale.Error:
        try:
            locale.setlocale(locale.LC_COLLATE, 'Korean_Korea.949')
        except locale.Error:
            # locale 설정 실패 시 그냥 기본정렬 사용
            pass

    # 소속정당 > 이름 순으로 한글 정렬
    members_sorted = sorted(
        members,
        key=lambda m: (
            locale.strxfrm(m.party),
            locale.strxfrm(m.name)
        )
    )

    paginator = Paginator(members_sorted, 15)
    page_number = request.GET.get('page')
    page_obj = paginator.get_page(page_number)
    return render(request, 'rate.html', {'page_obj': page_obj, 'query': query})

def local_view(request):
    query = request.GET.get('q', '')
    # 전체 쿼리셋 불러오기 (정렬 없이)
    if query:
        members = list(MemberInfo.objects.filter(
            elected_type='지역구',
            name__icontains=query
        ))
    else:
        members = list(MemberInfo.objects.filter(
            elected_type='지역구'
        ))
        
    try:
        locale.setlocale(locale.LC_COLLATE, 'ko_KR.utf8')
    except locale.Error:
        try:
            locale.setlocale(locale.LC_COLLATE, 'Korean_Korea.949')
        except locale.Error:
            # locale 설정 실패 시 그냥 기본정렬 사용
            pass
        
    members_sorted = sorted(
        members,
        key=lambda m: (
            locale.strxfrm(m.party),
            locale.strxfrm(m.name)
        )
    )
    
    paginator = Paginator(members_sorted, 15)
    page_number = request.GET.get('page')
    page_obj = paginator.get_page(page_number)
    return render(request, 'local.html', {'page_obj': page_obj, 'query': query})

def map_view(request):
    return render(request, 'map.html')

def detail_view(request, member_code):
    member = get_object_or_404(MemberInfo, member_code=member_code)
    return render(request, 'detail.html', {'member': member,})

def read_view(request):
    return render(request, 'read.html')

def social_view(request):
    return render(request, 'social.html')

def social2_view(request):
    return render(request, 'social2.html')
